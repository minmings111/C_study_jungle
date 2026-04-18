#include "csapp.h"  // 네트워크 프로그래밍에 필요한 함수와 자료형을 가져온다.

/*
 * echo 함수 선언:
 * echo 함수는 이 파일이 아니라 echo.c에 정의되어 있다.
 * C는 함수를 사용하기 전에 "이런 함수가 있다"는 선언을 먼저 알려 주는 경우가 많다.
 */
void echo(int connfd);

/*
 * main:
 * 서버 프로그램이 시작될 때 가장 먼저 실행되는 함수이다.
 *
 * 사용자가 포트 번호를 주면,
 * 그 포트에서 클라이언트 접속을 기다리는 리스닝 소켓을 만들고,
 * 접속이 들어올 때마다 echo 함수를 호출해서 받은 데이터를 다시 돌려준다.
 *
 * 매개변수 설명:
 * - argc:
 *   사용자가 터미널에서 입력한 "단어 개수"이다.
 *   포트 번호 자체가 아니라, 인자가 몇 개 들어왔는지를 저장한다.
 *   예를 들어 ./echoserveri 8080 으로 실행하면
 *   "./echoserveri", "8080" 이렇게 2개가 들어오므로 argc는 2가 된다.
 *
 * - argv:
 *   사용자가 입력한 각 인자 문자열을 담고 있는 배열이다.
 *   정확히는 "문자열들의 시작 주소를 저장한 배열"이다.
 *   argv[0]은 보통 프로그램 이름이고,
 *   argv[1]은 첫 번째 실제 인자이다.
 *
 * 그래서 ./echoserveri 8080 으로 실행하면:
 * - argv[0] = "./echoserveri"
 * - argv[1] = "8080"
 *
 * 왜 char **argv 인가:
 * - 문자열 하나는 보통 char *로 표현한다.
 * - argv는 문자열이 여러 개 들어 있으므로
 *   "char *가 여러 개 있는 배열"이 된다.
 * - 그래서 함수 매개변수 타입이 char **argv 이다.
 */
int main(int argc, char **argv) {
    int listenfd; // "서버가 손님을 기다릴 때 쓰는 소켓 번호", 소켓 번호는 운영체제가 정수로 관리하므로 int
    int connfd; // "실제로 한 클라이언트와 연결된 소켓 번호", 문자열이 아니라 소켓 번호이므로 int

    // socklen_t는 소켓 주소 구조체의 길이를 저장할 때 쓰는 전용 자료형
    // 주소 길이는 운영체제가 기대하는 형식에 맞추는 것이 중요해서 int 대신 socklen_t를 쓴다.
    socklen_t clientlen;

    // sockaddr_storage는 IPv4, IPv6 등 다양한 주소 형식을 담을 수 있게 넉넉하게 만들어 둔 구조체
    // 클라이언트 주소 전체를 저장해야 하므로 char 배열이 아니라 구조체를 쓴다.
    struct sockaddr_storage clientaddr;

    char client_host[MAXLINE]; // Getnameinfo가 사람이 읽을 수 있는 호스트 문자열을 넣어 줄 배열
    char client_port[MAXLINE]; // Getnameinfo가 사람이 읽을 수 있는 포트 문자열을 넣어 줄 배열

    // 서버는 포트 번호 하나만 받음. 프로그램 이름까지 포함하면 argc는 2여야 한다.
    // 여기서 argc는 "포트 번호"가 아니라 "인자 개수"
    // 예: "./echoserveri 8080"의 경우, 입력 단어는 2개이므로 argc == 2 
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]); // 인자를 잘못 넣었을 때 올바른 형식을 보여줌
        exit(0); // 포트가 없으면 서버를 열 수 없으므로 종료
    }

    // argv[1]에는 사용자가 입력한 포트 번호 문자열이 들어 있다.
    // 즉, 여기서는 "8080" 같은 문자열을 읽어서 사용하는 것이다.
    printf("[server] starting on port %s\n", argv[1]);

    // 지정한 포트에서 손님을 기다리는 리스닝 소켓을 만든다.
    // 성공하면 소켓 번호(int)가 반환되고, 그 값이 listenfd에 들어간다.
    listenfd = Open_listenfd(argv[1]);
    printf("[server] listening, waiting for clients...\n"); // 서버 준비 완료 알림

    // 서버는 보통 한 번 시작하면 계속 손님을 받아야 하므로 무한 반복
    while (1) {
        // Accept는 clientaddr 구조체의 크기도 함께 필요로 하므로 먼저 넣어 둔다.
        clientlen = sizeof(struct sockaddr_storage);

        // 클라이언트가 접속 시, "그 클라이언트와의 전용 연결 소켓 번호"를 connfd에 담아서 반환
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        // clientaddr 안의 주소 정보를
        // 사람이 읽을 수 있는 host 문자열과 port 문자열로 바꾼다.
        Getnameinfo((SA *)&clientaddr, clientlen, client_host, MAXLINE,
                    client_port, MAXLINE, 0);

        // 누가 접속했는지 확인하기 좋도록 출력한다.
        printf("[server] accepted connection from (%s, %s), connfd=%d\n",
               client_host, client_port, connfd);

        // 이제 실제 데이터 주고받기는 echo 함수가 담당한다.
        echo(connfd);

        // 현재 클라이언트와 통신이 끝났으므로 닫는다고 알려 준다.
        printf("[server] closing connfd=%d\n", connfd);

        // 현재 연결 전용 소켓을 닫는다.
        // listenfd는 계속 살아 있으므로 다음 손님도 받을 수 있다.
        Close(connfd);
    }
}
