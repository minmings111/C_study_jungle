#include <stdio.h>

/* 포인터 연습 문제 9
문제: text에서 pattern이 처음 시작하는 위치를 포인터만 사용해 찾으세요.
입력: text pattern
출력: 시작 인덱스: i
*/

char* findSubstringStart(const char* text, const char* pattern);

int main(void) {
  char text[256];
  char pattern[256];
  char* found;

  if (scanf("%255s %255s", text, pattern) != 2) return 0;

  found = findSubstringStart(text, pattern);
  if (found == NULL) {
    printf("시작 인덱스: -1\n");
  } else {
    printf("시작 인덱스: %ld\n", (long)(found - text));
  }
  return 0;
}

char* findSubstringStart(const char* text, const char* pattern) {
  // Todo: 포인터 비교만 사용해 pattern이 text에서 처음 시작하는 위치를 찾아 그
  // 주소를 반환하세요. 없으면 NULL을 반환하세요.
  return NULL;
}
