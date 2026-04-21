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

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg);

int main(int argc, char **argv) {
  int listenfd, connfd; // listenfd: 서버가 연결 요청을 기다리는 소켓 번호, connfd: 방금 연결된 클라이언트와 실제로 통신할 소켓 번호
  char hostname[MAXLINE], port[MAXLINE]; // Getnameinfo가 사람이 읽을 수 있는 클라이언트 호스트 문자열과 포트 문자열을 써 넣을 문자 배열
  socklen_t clientlen; // clientaddr 구조체의 길이를 담는 변수, 소켓 주소 길이 전용 타입이라 socklen_t를 사용
  struct sockaddr_storage clientaddr; // IPv4와 IPv6 주소를 모두 담을 수 있는 클라이언트 주소 구조체

  /* Check command line args */
  /* 명령줄 인자가 올바른지 확인 */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]);
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr,
                    &clientlen);  // line:netp:tiny:accept / 클라이언트 연결 요청을 받아서, 실제 통신용 소켓 번호를 connfd에 저장
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE,
                0);
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

  Rio_readinitb(&rio, fd);
  Rio_readlineb(&rio, buf, MAXLINE);
  
  printf("Request headers:\n");
  printf("%s", buf);
  sscanf(buf, "%s %s %s", method, uri, version);
  
  if(strcasecmp(method, "GET")){
    clienterror(fd, method, "501", "Not implemented", "Tiny does not implement this method");
    return ;
  }

  read_requesthdrs(&rio);



  is_static = parse_uri(uri, filename, cgiargs);
  if(stat(filename, &sbuf) < 0){
    clienterror(fd, filename, "404", "Not found", "Tiny couldn't find this file");
    return ; 
  }

  if(is_static){
    if(!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)){
      clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
      return ;
    }
    serve_static(fd, filename, sbuf.st_size);

  }
  else{
    if(!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)){
      clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program");
      return ;
    }
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
  sprintf(body, "<html><title> Tiny Error </title>");
  sprintf(body, "%s <body bgcolor = ""ffffff""> \r\n", body);
  sprintf(body, "%s%s: %s \r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s : %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr><em> The Tiny Web Server</em>\r\n", body);

  // print the http response / HTTP 응답을 클라이언트에게 보낸다
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));

  sprintf(buf, "Content-type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));

  sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
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

  Rio_readlineb(rp, buf, MAXLINE);
  while(strcmp(buf, "\r\n")){
    Rio_readlineb(rp, buf, MAXLINE);
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

  if(!strstr(uri, "cgi-bin")){
    strcpy(cgiargs, "");
    strcpy(filename, ".");
    strcat(filename, uri);

    if(uri[strlen(uri)-1] == '/'){
      strcat(filename, "home.html");
    }
    return 1;
  }
  else{
    ptr = index(uri, '?');
    if(ptr){
      strcpy(cgiargs, ptr+1);
      *ptr = '\0';
    }
    else{
      strcpy(cgiargs, "");
    }
    strcpy(filename, ".");
    strcat(filename, uri);
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

  getfiletype(filename, filetype);
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
  sprintf(buf, "%sConnection: close\r\n", buf);
  sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
  sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
  
  Rio_writen(fd, buf, strlen(buf));
  printf("Response headers: \n");
  printf("%s", buf);

  srcfd = Open(filename, O_RDONLY, 0);
  srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);

  Close(srcfd);

  Rio_writen(fd, srcp, filesize);
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
  if(strstr(filename, ".html")){
    strcpy(filetype, "text/html");
  }
  else if(strstr(filename, ".gif")){
    strcpy(filetype, "image/gif");
  }
  else if(strstr(filename, ".png")){
    strcpy(filetype, "image/png");
  }
  else if(strstr(filename, ".jpg")){
    strcpy(filetype, "image/jpg");
  }
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

  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));

  if(Fork() == 0){
    setenv("QUERY_STRING", cgiargs, 1);
    Dup2(fd, STDOUT_FILENO);
    Execve(filename, emptylist, environ);
  }
  Wait(NULL);
}
