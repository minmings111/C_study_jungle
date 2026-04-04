#include <stdio.h>

/* 포인터 연습 문제 1
문제: 두 정수를 포인터를 이용해 서로 교환하세요.
입력: 정수 2개
출력: 교환 후 값: a b
*/

void swapInts(int* a, int* b);

/*
포인터 빠른 정리표

표현      | 의미
----------|--------------------------------------------------
int x     | 정수값을 저장하는 일반 변수
&x        | 변수 x의 주소
int* p    | int의 주소를 저장하는 포인터 변수
*p        | p가 가리키는 주소 안의 실제 int 값
int** pp  | int*의 주소를 저장하는 이중 포인터 변수
*pp       | pp가 가리키는 int* 포인터
**pp      | 그 포인터가 최종적으로 가리키는 실제 int 값

짧은 예시
int x = 3;
int* p = &x;
int** pp = &p;

p   == &x
*p  == x
pp  == &p
*pp == p
**pp == x

핵심 기억
- int* 는 "int의 주소를 저장하는 변수"
- *a 는 "a가 들고 있는 주소로 가서 그 안의 값을 사용"
- &x 는 "x의 주소를 꺼냄"
*/

int main(void) {
  int a, b;

  if (scanf("%d %d", &a, &b) != 2) return 0;

  swapInts(&a, &b);
  printf("교환 후 값: %d %d\n", a, b);
  return 0;
}

void swapInts(int* a, int* b) {
  // Todo: 두 정수 포인터가 가리키는 값을 임시 변수를 사용해 서로 교환하세요.
  int temp = *a;
  *a = *b;
  *b = temp;
}
