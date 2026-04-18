#include "csapp.h"  // 네트워크 연결, 문자열 처리, 입출력 함수들을 쓰기 위해 포함한다.

/*
 * main:
 * 클라이언트 프로그램이 시작될 때 가장 먼저 실행되는 함수이다.
 *
 * argc는 명령줄 인자의 개수이고,
 * argv는 사용자가 입력한 인자 문자열들을 저장하고 있는 배열이다.
 * 이 프로그램은 host와 port를 받아 서버에 접속한 뒤,
 * 사용자가 입력한 문자열을 서버로 보내고 echo 응답을 다시 출력한다.
 *
 * 매개변수 설명:
 * - argc:
 *   사용자가 실행할 때 입력한 "단어 개수"이다.
 *   host도 아니고 port도 아니며, 인자가 총 몇 개인지만 저장한다.
 *
 * - argv:
 *   사용자가 입력한 각 문자열을 순서대로 담고 있는 배열이다.
 *   정확히는 "문자열 시작 주소들의 배열"이다.
 *
 * 예를 들어:
 * ./echoclient 127.0.0.1 8080
 * 로 실행하면
 * - argc = 3
 * - argv[0] = "./echoclient"
 * - argv[1] = "127.0.0.1"
 * - argv[2] = "8080"
 *
 * 왜 char **argv 인가:
 * - 문자열 하나는 char *로 표현한다.
 * - 그런데 argv 안에는 문자열이 여러 개 들어 있다.
 * - 따라서 "char *가 여러 개 들어 있는 배열"이므로 char **argv가 된다.
 */
int main(int argc, char **argv) {
    // clientfd는 서버와 연결된 소켓 번호이다.
    // 소켓도 운영체제가 정수 번호로 관리하므로 자료형이 int이다.
    int clientfd;

    // host는 문자열의 시작 주소를 가리킨다.
    // 예: "127.0.0.1"
    // 문자열 자체를 복사하지 않고 argv 안의 문자열을 가리키기만 하므로 char *를 쓴다.
    char *host;

    // port도 문자열이다.
    // 포트 번호는 숫자처럼 보여도, Open_clientfd 함수는 문자열 형태 인자를 받기 때문에 char *를 쓴다.
    char *port;

    // 사용자가 키보드로 입력한 문자열,
    // 그리고 서버가 다시 돌려준 문자열을 잠시 저장하는 공간이다.
    // 문자열 저장이 목적이므로 char 배열을 사용한다.
    char buf[MAXLINE];

    // rio_t는 "조금 더 안전하고 편한 읽기"를 위해 쓰는 구조체이다.
    // 한 글자씩이 아니라 한 줄 단위로 읽고 싶을 때 유용하다.
    rio_t rio;

    // argc는 명령줄 인자의 개수이다.
    // ./echoclient 127.0.0.1 8080 처럼 실행하면
    // argv[0] = 프로그램 이름, argv[1] = host, argv[2] = port 가 된다.
    // 따라서 총 개수는 3개여야 한다.
    //
    // 여기서 중요한 점:
    // - argc는 "127.0.0.1"이나 "8080" 같은 실제 값이 아니다.
    // - argc는 그냥 개수 3을 의미하는 정수이다.
    if (argc != 3) {
        // fprintf는 "형식(format)에 맞춰 출력"하는 함수이다.
        // stderr는 에러 메시지를 보여줄 때 자주 쓰는 출력 통로이다.
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);

        // 인자가 부족하면 더 진행할 수 없으므로 프로그램을 끝낸다.
        exit(0);
    }

    // argv[1]에는 사용자가 입력한 host 문자열의 시작 주소가 들어 있다.
    // 그래서 host는 그 문자열을 새로 복사하지 않고 그대로 가리키기만 한다.
    host = argv[1];

    // argv[2]에는 사용자가 입력한 port 문자열의 시작 주소가 들어 있다.
    // port 번호도 여기서는 정수가 아니라 문자열 "8080" 형태로 받는다.
    port = argv[2];

    // 어디로 접속하는지 먼저 보여 주면 흐름을 이해하기 쉽다.
    printf("[client] connecting to %s:%s\n", host, port);

    // 실제로 서버에 접속한다.
    // 접속에 성공하면 운영체제가 "이 연결은 몇 번 소켓이다"라는 번호를 돌려주는데,
    // 그 번호가 clientfd에 저장된다.
    clientfd = Open_clientfd(host, port);

    // 이제부터 rio가 clientfd 소켓에서 읽도록 준비한다.
    Rio_readinitb(&rio, clientfd);

    // 연결 성공 안내 메시지이다.
    printf("[client] connected, type messages and press enter\n");

    // Fgets는 표준 입력(stdin), 즉 키보드에서 한 줄씩 읽는다.
    // 더 이상 읽을 입력이 없을 때까지 반복한다.
    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        // strlen은 문자열 길이를 센다.
        // 길이는 "크기" 개념이므로 size_t 자료형에 담는다.
        size_t len = strlen(buf);

        // 지금 서버로 무엇을 보내는지 확인하기 위한 출력이다.
        printf("[client] sending %zu bytes: %s", len, buf);

        // 키보드에서 입력한 문자열을 서버로 보낸다.
        Rio_writen(clientfd, buf, len);

        // 서버가 다시 보내 준 한 줄을 읽는다.
        // 0이면 서버가 연결을 닫았다는 뜻이다.
        if (Rio_readlineb(&rio, buf, MAXLINE) == 0) {
            // 서버가 먼저 종료한 상황을 화면에 알려준다.
            printf("[client] server closed connection\n");

            // 더 이상 통신할 수 없으므로 반복을 끝낸다.
            break;
        }

        // 서버가 돌려준 응답을 그대로 보여 준다.
        printf("[client] received echo: %s", buf);
    }

    // 통신이 끝났으므로 소켓을 닫는다.
    Close(clientfd);

    // 0을 반환하면 보통 "정상 종료"라는 뜻이다.
    return 0;
}
