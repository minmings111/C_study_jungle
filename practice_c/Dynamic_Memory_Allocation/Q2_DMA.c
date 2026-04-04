#include <stdio.h>
#include <stdlib.h>

/* 동적 할당 연습 문제 2
문제: calloc으로 0으로 초기화된 배열을 만드세요.
입력: n
출력: 초기 배열: ...
*/

int* allocateZeroArray(int n);

int main(void) {
  int n;
  int* arr;
  int i;

  if (scanf("%d", &n) != 1) return 0;

  arr = allocateZeroArray(n);
  if (arr == NULL) {
    printf("초기 배열: 할당 실패\n");
    return 0;
  }

  printf("초기 배열:");
  for (i = 0; i < n; i++) printf(" %d", arr[i]);
  printf("\n");
  free(arr);
  return 0;
}

int* allocateZeroArray(int n) {
  // Todo: calloc을 사용해 n개의 int 배열을 만들고 그 주소를 반환하세요.
  
  return NULL;
}
