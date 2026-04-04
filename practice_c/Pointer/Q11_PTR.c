#include <stdio.h>

/* 포인터 연습 문제 11
문제: 두 문자열 포인터가 가리키는 대상을 서로 교환하세요.
난이도: 이중 포인터 기초
입력: 단어 2개
출력: 교환 후 문자열: s1 s2
*/

void swapStringPointers(char** s1, char** s2);

int main(void) {
  char first[101];
  char second[101];
  char* p1 = first;
  char* p2 = second;

  if (scanf("%100s %100s", first, second) != 2) return 0;

  swapStringPointers(&p1, &p2);
  printf("교환 후 문자열: %s %s\n", p1, p2);
  return 0;
}

void swapStringPointers(char** s1, char** s2) {
  // Todo: char**를 사용해 두 문자열 포인터가 가리키는 대상을 서로 교환하세요.
}
