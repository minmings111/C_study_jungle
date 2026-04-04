#include <stdio.h>

/* 포인터 연습 문제 2
문제: 두 정수의 합과 차를 출력 매개변수로 계산하세요.
입력: 정수 2개
출력: 합: x / 차: y
*/

void sumAndDiff(int a, int b, int* sum, int* diff);

/*
짧은 이해 포인트
- sum, diff는 int 변수의 주소를 받는 포인터 매개변수입니다.
- *sum은 sum이 가리키는 실제 변수값이고, *diff도 같은 방식입니다.
- 그래서 *sum = a + b; 는 함수 밖 main의 sum 값을 직접 바꾸는 코드입니다.
*/

int main(void) {
  int a, b;
  int sum = 0;
  int diff = 0;

  if (scanf("%d %d", &a, &b) != 2) return 0;

  sumAndDiff(a, b, &sum, &diff);
  printf("합: %d\n", sum);
  printf("차: %d\n", diff);
  return 0;
}

void sumAndDiff(int a, int b, int* sum, int* diff) {
  // Todo: sum이 가리키는 곳에는 a+b를, diff가 가리키는 곳에는 a-b를 저장하세요.
  *sum = a + b;
  *diff = a - b;
}
