#include <stdio.h>
#include <stdlib.h>

/* 동적 할당 연습 문제 6
문제: 입력 배열에서 짝수만 골라 새 동적 배열을 만드세요.
입력: n, 이어서 n개 정수
출력: 짝수 개수: c / 짝수 배열: ...
*/

int* filterEven(const int* arr, int n, int* outSize);

int main(void) {
  int n;
  int arr[100];
  int* evenArr;
  int outSize = 0;
  int i;

  if (scanf("%d", &n) != 1) return 0;
  for (i = 0; i < n; i++) scanf("%d", &arr[i]);

  evenArr = filterEven(arr, n, &outSize);
  printf("짝수 개수: %d\n", outSize);
  printf("짝수 배열:");
  for (i = 0; i < outSize; i++) printf(" %d", evenArr[i]);
  printf("\n");
  free(evenArr);
  return 0;
}

int* filterEven(const int* arr, int n, int* outSize) {
  // Todo: 먼저 짝수 개수를 세고, 그 크기만큼 메모리를 할당한 뒤 짝수만 복사해 반환하세요.
  if (outSize != NULL) *outSize = 0;
  return NULL;
}
