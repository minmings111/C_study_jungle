#include "csapp.h"  // 소켓, 문자열, 입출력 함수 등을 한 번에 쓰기 위해 포함한다.

/*
 * echo:
 * 클라이언트가 보낸 데이터를 "그대로 다시" 돌려주는 함수이다.
 *
 * 중요한 점:
 * - 이 파일은 서버를 직접 실행하는 파일이 아니다.
 * - 이 함수는 echoserveri.c 안의 main이 필요할 때 호출하는 "작업 함수"이다.
 * - 즉, 서버가 실제로 접속을 받은 뒤에
 *   그 연결(connfd)을 echo()에 넘겨 주면,
 *   echo()가 그 안에서 사용자의 입력을 읽고 그대로 다시 보내 준다.
 *
 * connfd는 "사용자가 입력한 문자열"이 아니라,
 * 이미 연결이 끝난 소켓을 가리키는 정수 번호(file descriptor)이다.
 * 운영체제가 열린 파일/소켓마다 번호를 붙여 관리하는데,
 * 소켓도 파일처럼 다루기 때문에 자료형이 문자열(char *)이 아니라 int이다.
 */
void echo(int connfd) {
    // size_t는 "메모리 크기"나 "읽은 바이트 수"를 저장할 때 쓰는 자료형이다.
    // read 계열 함수는 몇 바이트를 읽었는지 돌려주므로,
    // 음수가 아닌 크기 값을 자연스럽게 담을 수 있는 size_t를 사용한다.
    size_t n;

    // char는 문자 1개를 저장하는 자료형이다.
    // char 배열은 문자열을 저장할 수 있으므로,
    // 클라이언트가 보낸 한 줄의 텍스트를 담기 위해 char buf[]를 사용한다.
    char buf[MAXLINE];

    // rio_t는 CS:APP에서 만든 "버퍼 입출력 전용 구조체" 타입이다.
    // 단순 int나 char 배열로는 읽기 상태를 관리하기 어려워서,
    // 내부 버퍼, 현재 읽은 위치 같은 정보를 구조체 하나에 모아 둔다.
    rio_t rio;

    // 지금 어떤 연결 번호(connfd)에 대해 echo 함수가 실행되는지 출력한다.
    printf("[echo] handler started for connfd=%d\n", connfd);

    // rio 구조체가 connfd 소켓에서 읽도록 초기 설정을 한다.
    // 즉, "이 rio는 이 소켓에서 읽어라"라고 연결해 주는 단계이다.
    Rio_readinitb(&rio, connfd);

    // Rio_readlineb는 한 줄을 읽는다.
    // 반환값은 "실제로 읽은 바이트 수"이다.
    // 0이 나오면 보통 상대방(클라이언트)이 연결을 끊었다는 뜻이다.
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        // 서버가 실제로 받은 문자열과 바이트 수를 확인한다.
        printf("[echo] received %zu bytes: %s", n, buf);

        // 방금 받은 데이터를 그대로 같은 소켓(connfd)으로 다시 보낸다.
        Rio_writen(connfd, buf, n);

        // 몇 바이트를 되돌려 보냈는지 출력한다.
        printf("[echo] sent back %zu bytes\n", n);
    }

    // while문이 끝났다면 더 이상 읽을 데이터가 없다는 뜻이고,
    // 이 예제에서는 보통 클라이언트가 연결을 종료했다는 의미로 보면 된다.
    printf("[echo] client closed connection on connfd=%d\n", connfd);
}
