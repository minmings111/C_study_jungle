/* $begin tinymain */
/*
 * tiny.c - A simple, iterative HTTP/1.0 Web server that uses the
 *     GET method to serve static and dynamic content.
 * tiny.c - 정적 콘텐츠와 동적 콘텐츠를 제공하기 위해
 *     GET 메서드를 사용하는 간단한 반복형 HTTP/1.0 웹 서버이다.
 *
 * Updated 11/2019 droh
 *   - Fixed sprintf() aliasing issue in serve_static(), and clienterror().
 * 2019년 11월 수정
 *   - serve_static()과 clienterror()에서 sprintf 별칭 문제를 수정했다.
 */
#include "csapp.h"

// 클라이언트 요청 1개를 끝까지 처리하는 핵심 함수 선언
void doit(int fd);
// 요청 헤더를 끝까지 읽어서 소비하는 함수 선언
void read_requesthdrs(rio_t *rp);
// URI를 분석해 정적/동적 요청을 구분하는 함수 선언
int parse_uri(char *uri, char *filename, char *cgiargs);
// 정적 파일을 읽어 HTTP 응답으로 보내는 함수 선언
void serve_static(int fd, char *filename, int filesize);
// 파일 확장자에 맞는 Content-type을 결정하는 함수 선언
void get_filetype(char *filename, char *filetype);
// CGI 프로그램을 실행해 동적 응답을 보내는 함수 선언
void serve_dynamic(int fd, char *filename, char *cgiargs);
// 에러 상황에서 HTTP 에러 응답을 만들어 보내는 함수 선언
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

int main(int argc, char **argv) {
  int listenfd, connfd; // listenfd: 서버가 연결 요청을 기다리는 소켓 번호, connfd: 방금 연결된 클라이언트와 실제로 통신할 소켓 번호
  char hostname[MAXLINE], port[MAXLINE]; // Getnameinfo가 사람이 읽을 수 있는 클라이언트 호스트 문자열과 포트 문자열을 써 넣을 문자 배열
  socklen_t clientlen; // clientaddr 구조체의 길이를 담는 변수, 소켓 주소 길이 전용 타입이라 socklen_t를 사용
  struct sockaddr_storage clientaddr; // IPv4와 IPv6 주소를 모두 담을 수 있는 클라이언트 주소 구조체

  /* Check command line args */
  /* 명령줄 인자가 올바른지 확인 */
  if (argc != 2) {
    // 포트 번호를 주지 않았으면 올바른 실행 형식을 에러 출력으로 보여 준다.
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    // 인자가 잘못됐으므로 프로그램을 즉시 종료한다.
    exit(1);
  }

  // 사용자가 넘긴 포트 번호 문자열로 리스닝 소켓을 연다.
  listenfd = Open_listenfd(argv[1]); // 사용자가 입력한 1번째 실제 인자가 포트 번호
  // 반복형(iterative) 서버이므로 한 번에 하나씩 연결을 받아 계속 처리한다.
  while (1) {
    // Accept가 clientaddr에 주소를 채울 수 있도록 구조체 크기를 미리 넣어 둔다.
    clientlen = sizeof(clientaddr);
    // line:netp:tiny:accept / 클라이언트 연결 요청을 받아서, 실제 통신용 소켓 번호를 connfd에 저장
    // clientaddr의 실제 타입은 sockaddr_storage지만, Accept는 sockaddr * 타입을 요구하므로
    // 공통 소켓 주소 타입인 (SA *)로 형변환해서 넘긴다.
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    
    // 클라이언트 주소 정보를 사람이 읽을 수 있는 문자열로 바꿔줌
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
    // 어떤 클라이언트가 연결됐는지 서버 터미널에 출력한다.
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    
    doit(connfd);   // line:netp:tiny:doit / 방금 연결된 클라이언트의 요청 1개를 처리
    Close(connfd);  // line:netp:tiny:close / 현재 클라이언트와의 연결을 닫음
  }
}


/*
1개의 http 트랜잭션 처리

- 이 함수는 클라이언트가 보낸 HTTP 요청 1개를 끝까지 처리한다.
- 먼저 요청줄과 헤더를 읽고,
- 요청한 URI가 정적 콘텐츠인지 동적 콘텐츠인지 구분한 뒤,
- 알맞은 함수(serve_static 또는 serve_dynamic)를 호출한다.

매개변수:
- fd
  - 현재 클라이언트와 연결된 소켓 번호이다.
  - 왜 int 타입인가:
    운영체제는 파일, 터미널, 소켓 같은 입출력 대상을
    파일 디스크립터라는 정수 번호로 관리하기 때문이다.
  - 이 함수는 이 fd를 통해 클라이언트 요청을 읽고,
    응답도 같은 fd를 통해 다시 보낸다.

반환값:
- 반환값이 없는 void 함수이다.
- 이유:
  - 요청 처리 결과를 다른 값으로 돌려주기보다
    이 함수 안에서 바로 클라이언트에게 응답을 보내는 방식으로 동작하기 때문이다.
  - 에러가 나면 clienterror()를 호출해서 즉시 에러 응답을 보낸 뒤 종료한다.
*/
void doit(int fd){
  int is_static; // parse_uri의 결과를 저장, 정적 콘텐츠면 1이고 동적 콘텐츠면 0
  struct stat sbuf; // 요청한 파일의 크기, 권한, 일반 파일 여부 등을 stat()가 채워 넣을 구조체
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE], filename[MAXLINE], cgiargs[MAXLINE]; // 요청 한 줄, 메서드, URI, HTTP 버전, 실제 파일 경로, CGI 인자를 저장할 문자열 버퍼들
  rio_t rio; // fd에서 한 줄씩 읽기 위해 현재 읽기 상태를 저장하는 Rio 구조체

  // 이 연결 fd로부터 버퍼 기반 입력을 읽을 수 있게 rio를 초기화한다.
  Rio_readinitb(&rio, fd);
  // 요청의 첫 줄(Request-Line)을 읽는다. 예: GET /home.html HTTP/1.0
  Rio_readlineb(&rio, buf, MAXLINE);
  
  // 이제부터 요청 내용 출력이 시작됨을 알려 준다.
  printf("Request headers:\n");
  // 방금 읽은 요청 첫 줄을 서버 터미널에 출력한다.
  printf("%s", buf);
  
  // 요청 첫 줄에서 메서드, URI, HTTP 버전을 각각 분리해 저장한다.
  sscanf(buf, "%s %s %s", method, uri, version);
  
  // Tiny는 GET만 구현하므로 다른 메서드면 501 에러를 보낸다.
  if(strcasecmp(method, "GET")){
    // 지원하지 않는 메서드 이름을 원인(cause)으로 넣어 에러 응답을 보낸다.
    clienterror(fd, method, "501", "Not implemented", "Tiny does not implement this method");
    // 에러 응답을 보냈으니 현재 요청 처리를 끝낸다.
    return ;
  }

  // 첫 줄 다음에 오는 나머지 요청 헤더를 끝까지 읽는다.
  read_requesthdrs(&rio);

  // URI를 보고 정적 파일 요청인지 CGI 실행 요청인지 판별한다.
  is_static = parse_uri(uri, filename, cgiargs);
  // 요청 대상 파일이 실제로 존재하는지 검사한다.
  if(stat(filename, &sbuf) < 0){
    // stat 실패는 보통 파일이 없다는 뜻이므로 404를 보낸다.
    clienterror(fd, filename, "404", "Not found", "Tiny couldn't find this file");
    // 더 진행할 수 없으므로 요청 처리를 종료한다.
    return ; 
  }

  // 정적 콘텐츠 요청이면 읽기 가능한 일반 파일인지 확인한 뒤 전송한다.
  if(is_static){
    // 일반 파일이 아니거나 읽기 권한이 없으면 403 응답을 보낸다.
    if(!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)){
      clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
      return ;
    }
    // 조건을 통과한 정적 파일을 클라이언트에게 전송한다.
    serve_static(fd, filename, sbuf.st_size);

  }
  // 동적 콘텐츠 요청이면 실행 가능한 일반 파일인지 확인한 뒤 CGI를 실행한다.
  else{
    // 일반 파일이 아니거나 실행 권한이 없으면 CGI를 돌릴 수 없으므로 403을 보낸다.
    if(!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)){
      clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program");
      return ;
    }
    // 실행 가능한 CGI 프로그램을 호출해 동적 응답을 만든다.
    serve_dynamic(fd, filename, cgiargs);
  }

}

/*
error message를 클라이언트에게 출력

- HTTP 에러 응답을 만들어서 클라이언트에게 전송하는 함수이다.
- 예를 들어 404 Not found, 403 Forbidden 같은 응답을 보낼 때 사용한다.

매개변수:
- fd
  - 에러 응답을 보낼 대상 클라이언트 소켓 번호이다.
  - int인 이유는 소켓이 정수 파일 디스크립터로 관리되기 때문이다.
- cause
  - 어떤 대상 때문에 에러가 났는지 설명하는 문자열이다.
  - 예: 요청한 파일 이름
- errnum
  - HTTP 상태 코드 문자열이다.
  - 예: "404", "403", "501"
- shortmsg
  - 짧은 에러 이름 문자열이다.
  - 예: "Not found"
- longmsg
  - 조금 더 자세한 설명 문자열이다.

왜 문자열 타입이 char *인가:
- 이 값들은 모두 텍스트 메시지이기 때문이다.
- 함수는 이미 만들어져 있는 문자열을 받아서 응답 본문에 조합해 넣는다.

반환값:
- 반환값이 없는 void 함수이다.
- 이유:
  - 에러 메시지를 만드는 즉시 클라이언트에게 전송하는 역할만 하고 끝나기 때문이다.
*/
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg){
  char buf[MAXLINE], body[MAXBUF]; // buf: HTTP 응답 헤더 한 줄을 만드는 버퍼, body: 클라이언트에게 보낼 HTML 에러 본문 전체

  // build the http response body / HTTP 응답 본문을 만든다
  // HTML 문서의 시작 부분과 제목을 만든다.
  sprintf(body, "<html><title> Tiny Error </title>");
  // 본문 배경색과 body 태그를 이어 붙인다.
  sprintf(body, "%s <body bgcolor = ""ffffff""> \r\n", body);
  // 상태 코드와 짧은 에러 메시지를 본문에 넣는다.
  sprintf(body, "%s%s: %s \r\n", body, errnum, shortmsg);
  // 더 자세한 설명과 원인이 된 대상을 본문에 넣는다.
  sprintf(body, "%s<p>%s : %s\r\n", body, longmsg, cause);
  // 서버 서명을 덧붙여 에러 페이지를 마무리한다.
  sprintf(body, "%s<hr><em> The Tiny Web Server</em>\r\n", body);

  // print the http response / HTTP 응답을 클라이언트에게 보낸다
  // 상태 줄을 먼저 전송한다. 예: HTTP/1.0 404 Not found
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  // 지금 만든 상태 줄 문자열을 소켓으로 보낸다.
  Rio_writen(fd, buf, strlen(buf));

  // 본문 타입이 HTML임을 알리는 헤더를 보낸다.
  sprintf(buf, "Content-type: text/html\r\n");
  // Content-type 헤더 한 줄을 전송한다.
  Rio_writen(fd, buf, strlen(buf));

  // 본문 길이를 알려 주는 헤더 뒤에 빈 줄을 보내 헤더를 끝낸다.
  sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
  // Content-length 헤더와 헤더 종료용 빈 줄을 전송한다.
  Rio_writen(fd, buf, strlen(buf));
  // 마지막으로 실제 HTML 본문을 전송한다.
  Rio_writen(fd, body, strlen(body));

}

/*
요청 헤더를 읽고 무시

- 요청 헤더의 남은 줄들을 빈 줄이 나올 때까지 읽는다.
- Tiny 예제에서는 요청 헤더 내용을 따로 저장해서 활용하지 않기 때문에,
  읽기만 하고 넘어간다.

매개변수:
- rp
  - 현재 연결에 대한 Rio 읽기 상태 구조체 주소이다.
  - 왜 rio_t * 타입인가:
    Rio 패키지는 읽기 상태를 구조체에 저장해서 관리하고,
    함수가 그 구조체 안의 상태를 갱신해야 하므로 주소를 넘겨야 한다.

반환값:
- 반환값이 없는 void 함수이다.
- 이유:
  - 헤더를 읽어서 소비하는 작업만 수행하고,
    필요한 결과는 다음 읽기 위치가 rp 안에 반영되는 것으로 충분하기 때문이다.
*/
void read_requesthdrs(rio_t *rp){
  char buf[MAXLINE]; // 요청 헤더 한 줄을 임시로 담는 문자열 버퍼

  // 첫 번째 헤더 줄을 먼저 읽는다.
  Rio_readlineb(rp, buf, MAXLINE);
  // 빈 줄("\r\n")이 나올 때까지 헤더 줄을 계속 읽는다.
  while(strcmp(buf, "\r\n")){
    // 다음 헤더 줄을 읽는다.
    Rio_readlineb(rp, buf, MAXLINE);
    // 학습용으로 서버 터미널에 읽은 헤더를 그대로 출력한다.
    printf("%s", buf);
  }
  return ;
}

/*
http uri 분석

- 요청 URI를 보고 정적 콘텐츠 요청인지 동적 콘텐츠 요청인지 구분한다.
- 정적이면 실제 파일 경로를 filename에 만들고,
- 동적이면 CGI 인자를 cgiargs에 분리해서 저장한다.

매개변수:
- uri
  - 클라이언트가 요청한 URI 문자열이다.
  - 예: "/", "/home.html", "/cgi-bin/adder?1&2"
- filename
  - 실제 파일 시스템 경로를 저장할 출력용 문자열 버퍼이다.
- cgiargs
  - CGI 프로그램에 넘길 쿼리 문자열을 저장할 출력용 문자열 버퍼이다.

왜 이 매개변수들이 char * 타입인가:
- 모두 문자열 데이터를 다루기 때문이다.
- 특히 filename과 cgiargs는
  함수 안에서 내용을 채워 넣어야 하므로
  "문자열 버퍼의 시작 주소"를 전달받는다.

반환값:
- int를 반환한다.
- 반환 의미:
  - 1: 정적 콘텐츠
  - 0: 동적 콘텐츠
- 즉, 호출한 쪽은 이 반환값을 보고
  serve_static을 부를지 serve_dynamic을 부를지 결정한다.
*/
int parse_uri(char *uri, char *filename, char *cgiargs){
  char *ptr; // URI 안의 '?' 위치를 가리킬 포인터, CGI 인자 시작 지점을 찾을 때 사용

  // URI 안에 "cgi-bin"이 없으면 정적 콘텐츠 요청으로 본다.
  if(!strstr(uri, "cgi-bin")){
    // 정적 요청에는 CGI 인자가 없으므로 빈 문자열로 둔다.
    strcpy(cgiargs, "");
    // 실제 파일 경로를 현재 디렉터리 기준으로 만들기 위해 "."부터 시작한다.
    strcpy(filename, ".");
    // 요청 URI를 뒤에 이어 붙여 파일 경로를 완성한다.
    strcat(filename, uri);

    // URI가 "/"로 끝나면 기본 페이지인 home.html을 붙인다.
    if(uri[strlen(uri)-1] == '/'){
      strcat(filename, "home.html");
    }
    // 1은 정적 콘텐츠라는 뜻이다.
    return 1;
  }
  else{
    // 동적 요청이면 '?' 위치를 찾아 CGI 인자와 실행 파일 경로를 분리한다.
    ptr = index(uri, '?');
    if(ptr){
      // '?' 뒤의 문자열을 CGI 인자로 복사한다.
      strcpy(cgiargs, ptr+1);
      // 원래 URI 문자열에서 '?'를 문자열 끝으로 바꿔 파일 경로 부분만 남긴다.
      *ptr = '\0';
    }
    else{
      // 쿼리 문자열이 없으면 CGI 인자는 빈 문자열이다.
      strcpy(cgiargs, "");
    }
    // CGI 프로그램 파일 경로도 현재 디렉터리 기준으로 만든다.
    strcpy(filename, ".");
    // "/cgi-bin/adder" 같은 URI를 이어 붙여 실행 파일 경로를 완성한다.
    strcat(filename, uri);
    // 0은 동적 콘텐츠라는 뜻이다.
    return 0;
  }
}

/*
정적 컨텐츠를 클라이언트에게 서비스

- 정적 파일(HTML, 이미지 등)을 읽어서 HTTP 응답으로 보내는 함수이다.

매개변수:
- fd
  - 응답을 보낼 클라이언트 소켓 번호이다.
- filename
  - 클라이언트가 요청한 실제 파일 경로 문자열이다.
- filesize
  - 해당 파일 크기이다.

왜 타입이 이런가:
- fd는 소켓 번호이므로 int
- filename은 파일 경로 문자열이므로 char *
- filesize는 파일 크기 숫자이므로 int

반환값:
- 반환값이 없는 void 함수이다.
- 이유:
  - 정적 파일을 읽어서 응답 헤더와 본문을 바로 전송하는 역할만 하기 때문이다.
*/
void serve_static(int fd, char *filename, int filesize){
  int srcfd; // 정적 파일을 열었을 때 돌려받는 파일 디스크립터
  char *srcp, filetype[MAXLINE], buf[MAXBUF]; // srcp: mmap으로 매핑된 파일 내용의 시작 주소, filetype: Content-type 문자열, buf: 응답 헤더를 조합하는 버퍼

  // 파일 이름 확장자를 보고 MIME 타입을 결정한다.
  get_filetype(filename, filetype);
  // 상태 줄을 작성한다.
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  // 서버 이름 헤더를 덧붙인다.
  sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
  // 이 응답 후 연결을 닫는다는 헤더를 덧붙인다.
  sprintf(buf, "%sConnection: close\r\n", buf);
  // 본문 길이를 Content-length에 기록한다.
  sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
  // 파일 형식에 맞는 Content-type을 넣고 빈 줄로 헤더를 끝낸다.
  sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
  
  // 완성한 응답 헤더를 클라이언트에게 보낸다.
  Rio_writen(fd, buf, strlen(buf));
  printf("Response headers: \n");
  // 학습용으로 서버 터미널에도 동일한 응답 헤더를 출력한다.
  printf("%s", buf);

  // 요청된 정적 파일을 읽기 전용으로 연다.
  srcfd = Open(filename, O_RDONLY, 0);
  // 파일 전체를 메모리에 매핑해 한 번에 전송하기 쉽게 만든다.
  srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);

  // mmap이 끝났으므로 파일 디스크립터는 닫아도 된다.
  Close(srcfd);

  // 매핑된 파일 내용을 응답 본문으로 클라이언트에게 전송한다.
  Rio_writen(fd, srcp, filesize);
  // 전송이 끝났으니 매핑한 메모리를 해제한다.
  Munmap(srcp, filesize);
}

/*
파일 이름 확장자에 맞는 Content-type 문자열 결정

- 파일 이름을 보고 HTTP 응답 헤더에 넣을 Content-type 값을 정한다.

매개변수:
- filename
  - 확장자를 검사할 파일 이름 문자열이다.
- filetype
  - 결정된 Content-type 문자열을 저장할 출력용 버퍼이다.

왜 char * 타입인가:
- 둘 다 문자열을 다루기 때문이다.
- filetype은 함수 안에서 내용을 써 넣어야 하므로
  문자열 버퍼의 시작 주소를 받는다.

반환값:
- 반환값이 없는 void 함수이다.
- 이유:
  - 결과를 반환값으로 주는 대신 filetype 버퍼에 직접 써 주기 때문이다.
 */
void get_filetype(char *filename, char *filetype){
  // 파일 이름에 ".html"이 들어 있으면 HTML 문서로 본다.
  if(strstr(filename, ".html")){
    strcpy(filetype, "text/html");
  }
  // GIF 확장자면 GIF 이미지 타입으로 지정한다.
  else if(strstr(filename, ".gif")){
    strcpy(filetype, "image/gif");
  }
  // PNG 확장자면 PNG 이미지 타입으로 지정한다.
  else if(strstr(filename, ".png")){
    strcpy(filetype, "image/png");
  }
  // JPG 확장자면 JPG 이미지 타입으로 지정한다.
  else if(strstr(filename, ".jpg")){
    strcpy(filetype, "image/jpg");
  }
  // 위에 해당하지 않으면 일반 텍스트로 간주한다.
  else{
    strcpy(filetype, "text/plain");
  }
}

/*
동적컨텐츠를 클라이언트에게 제공

- CGI 프로그램을 실행해서 동적 콘텐츠를 클라이언트에게 보낸다.

매개변수:
- fd
  - 응답을 보낼 클라이언트 소켓 번호이다.
- filename
  - 실행할 CGI 프로그램 파일 경로 문자열이다.
- cgiargs
  - CGI 프로그램에 넘길 쿼리 문자열이다.

왜 타입이 이런가:
- fd는 소켓 번호라서 int
- filename과 cgiargs는 문자열이라서 char *

반환값:
- 반환값이 없는 void 함수이다.
- 이유:
  - CGI 프로그램 실행과 응답 전송을 함수 안에서 바로 끝내기 때문이다.
*/
void serve_dynamic(int fd, char *filename, char *cgiargs){
  char buf[MAXLINE], *emptylist[] = { NULL }; // buf: 간단한 HTTP 응답 헤더를 담는 버퍼, emptylist: CGI 프로그램에 넘길 비어 있는 인자 목록

  // 동적 응답도 먼저 200 OK 상태 줄부터 보낸다.
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  // 상태 줄을 먼저 클라이언트에게 전송한다.
  Rio_writen(fd, buf, strlen(buf));
  // 서버 이름 헤더를 보낸다. 본문과 나머지 헤더는 CGI 프로그램이 출력한다.
  sprintf(buf, "Server: Tiny Web Server\r\n");
  // Tiny 자체가 보내는 마지막 헤더 줄을 전송한다.
  Rio_writen(fd, buf, strlen(buf));

  // CGI 프로그램 실행은 자식 프로세스가 담당하게 한다.
  if(Fork() == 0){
    // CGI 표준에 맞게 쿼리 문자열을 환경 변수 QUERY_STRING으로 넘긴다.
    setenv("QUERY_STRING", cgiargs, 1);
    // CGI 프로그램의 표준 출력이 곧 클라이언트 소켓으로 나가게 연결한다.
    Dup2(fd, STDOUT_FILENO);
    // CGI 실행 파일을 현재 프로세스 이미지 대신 실행한다.
    Execve(filename, emptylist, environ);
  }
  // 부모 프로세스는 자식 CGI가 끝날 때까지 기다린다.
  Wait(NULL);
}
