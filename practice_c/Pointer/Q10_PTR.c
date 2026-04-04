#include <stdio.h>

/* 포인터 연습 문제 10
문제: char** cursor를 이용해 콤마로 구분된 토큰을 하나씩 잘라 반환하세요.
입력: 쉼표로 구분된 문자열 1개
출력: 각 토큰을 한 줄씩 출력
*/

char* nextToken(char** cursor, char delimiter);

int main(void) {
  char buffer[256];
  char* cursor;
  char* token;
  int count = 0;

  if (scanf("%255s", buffer) != 1) return 0;

  cursor = buffer;
  while ((token = nextToken(&cursor, ',')) != NULL) {
    printf("토큰 %d: %s\n", ++count, token);
  }
  return 0;
}

char* nextToken(char** cursor, char delimiter) {
  // Todo: *cursor가 가리키는 위치에서 다음 delimiter 또는 문자열 끝까지를 토큰으로 분리하세요.
  // 토큰 시작 주소를 반환하고, 다음 호출을 위해 *cursor를 다음 위치로 이동시키세요. 더 이상 토큰이 없으면 NULL을 반환하세요.
  return NULL;
}
