#include <stdio.h>

/* 포인터 연습 문제 8
문제: int* 포인터 배열을 각 포인터가 가리키는 값 기준으로 오름차순 정렬하세요.
입력: n, 이어서 n개 정수
출력: 정렬된 포인터 값: ...
*/

void sortIntPointers(int* ptrs[], int n);

int main(void) {
  int n;
  int values[100];
  int* ptrs[100];
  int i;

  if (scanf("%d", &n) != 1) return 0;
  for (i = 0; i < n; i++) {
    scanf("%d", &values[i]);
    ptrs[i] = &values[i];
  }

  sortIntPointers(ptrs, n);

  printf("정렬된 포인터 값:");
  for (i = 0; i < n; i++) printf(" %d", *ptrs[i]);
  printf("\n");
  return 0;
}

void sortIntPointers(int* ptrs[], int n) {
  // Todo: 포인터 배열 자체를 정렬하되, 비교 기준은 각 포인터가 가리키는 값으로 삼으세요.
}
