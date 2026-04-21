/*
 * adder.c - a minimal CGI program that adds two numbers together
 */
/* $begin adder */
// getenv, atoi, printf 같은 함수와 MAXLINE 상수를 쓰기 위해 csapp.h를 포함한다.
#include "csapp.h"

int main(void) {
  // buf는 QUERY_STRING 전체를 가리키고, p는 '&' 위치를 가리키는 포인터이다.
  char *buf, *p;
  // arg1, arg2는 문자열 형태의 두 숫자를 저장하고, content는 응답 본문 전체를 담는다.
  char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
  // n1, n2는 문자열 숫자를 실제 정수로 바꿔 저장하는 변수이다.
  int n1 = 0, n2 = 0;

  // extract the two arguments
  // CGI 환경 변수 QUERY_STRING을 읽는다. 예: "1&2"
  if((buf = getenv("QUERY_STRING")) != NULL) {
    // 두 숫자를 구분하는 '&' 문자의 위치를 찾는다.
    p = strchr(buf, '&');
    // '&'를 문자열 끝으로 바꿔 앞부분을 첫 번째 숫자 문자열로 만든다.
    *p = '\0';
    // 첫 번째 숫자 문자열을 arg1에 복사한다.
    strcpy(arg1, buf);
    // '&' 다음 문자열을 두 번째 숫자 문자열로 복사한다.
    strcpy(arg2, p+1);
    // 첫 번째 숫자 문자열을 정수로 변환한다.
    n1 = atoi(arg1);
    // 두 번째 숫자 문자열을 정수로 변환한다.
    n2 = atoi(arg2);
  }

  //make the response body
  // 응답 본문 전체를 한 번에 안전하게 만든다.
  snprintf(content, sizeof(content),
           "QUERY_STRING=%s\r\n"
           "Welcom to add.com: "
           "The Internet addition partal.\r\n<p>"
           "The answer is: %d + %d = %d\r\n<p>"
           "Thanks for visiting!\r\n",
           buf, n1, n2, n1+n2);

  // generate the http response
  // 응답 후 연결을 닫겠다는 헤더를 출력한다.
  printf("Connection: close\r\n");
  // 응답 본문 길이를 Content-length 헤더로 출력한다.
  printf("Content-length: %d\r\n", (int)strlen(content));
  // 본문 타입이 HTML임을 알리고, 빈 줄로 헤더 구간을 끝낸다.
  printf("Content-type: text/html\r\n\r\n");
  // 실제 응답 본문 HTML 텍스트를 출력한다.
  printf("%s", content);
  
  // stdout 버퍼를 즉시 비워서 웹 서버 쪽으로 응답이 확실히 전달되게 한다.
  fflush(stdout);

  // CGI 프로그램을 정상 종료한다.
  exit(0);
}
/* $end adder */
