#include <stdio.h>

/* 포인터 연습 문제 5
문제: 문자열에서 특정 문자가 몇 번 등장하는지 세세요.
입력: 문자열 1개, 문자 1개
출력: 개수: c
*/

int countChar(const char* str, char target);

int main(void) {
  char str[256];
  char target;

  if (scanf("%255s %c", str, &target) != 2) return 0;

  printf("개수: %d\n", countChar(str, target));
  return 0;
}

int countChar(const char* str, char target) {
  // Todo: 문자 포인터를 한 칸씩 이동시키며 target과 같은 문자의 개수를 세어 반환하세요.
  return 0;
}
