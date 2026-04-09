#include <stdio.h>

/* 포인터 연습 문제 7
문제: 두 정수 포인터가 가리키는 대상을 교환하세요.
입력: 정수 2개
출력: 포인터가 가리키는 값: x y
*/

void swapIntPointers(int** p1, int** p2);

int main(void) {
  int a, b;
  int* p = &a;
  int* q = &b;

  if (scanf("%d %d", &a, &b) != 2) return 0;

  swapIntPointers(&p, &q);
  printf("포인터가 가리키는 값: %d %d\n", *p, *q);
  return 0;
}

void swapIntPointers(int** p1, int** p2) {
  // Todo: 이중 포인터를 사용해 p1과 p2가 가리키는 int* 대상을 서로 교환하세요.
  int *temp = *p1;
  *p1 = *p2;
  *p2 = temp;
}
