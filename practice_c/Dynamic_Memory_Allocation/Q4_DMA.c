#include <stdio.h>
#include <stdlib.h>

/* 동적 할당 연습 문제 4
문제: 문자열을 동적으로 복제하세요.
입력: 문자열 1개
출력: 복제 문자열: text
*/

char* duplicateString(const char* src);

int main(void) {
  char src[256];
  char* copy;

  if (scanf("%255s", src) != 1) return 0;

  copy = duplicateString(src);
  if (copy == NULL) {
    printf("복제 문자열: 할당 실패\n");
    return 0;
  }

  printf("복제 문자열: %s\n", copy);
  free(copy);
  return 0;
}

char* duplicateString(const char* src) {
  // Todo: 문자열 길이만큼 메모리를 할당하고, 마지막 널 문자까지 복사한 새 문자열을 반환하세요.
  return NULL;
}
