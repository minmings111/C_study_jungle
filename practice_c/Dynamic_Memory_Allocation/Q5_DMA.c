#include <stdio.h>
#include <stdlib.h>

/* 동적 할당 연습 문제 5
문제: 배열을 realloc으로 한 칸 늘리고 새 값을 뒤에 추가하세요.
입력: n, n개 정수, 추가할 값 1개
출력: 확장 배열: ...
*/

int* appendValue(int* arr, int oldSize, int value);

int main(void) {
  int n;
  int value;
  int* arr;
  int i;

  if (scanf("%d", &n) != 1) return 0;
  arr = (int*)malloc(sizeof(int) * n);
  if (arr == NULL) return 0;
  for (i = 0; i < n; i++) scanf("%d", &arr[i]);
  scanf("%d", &value);

  arr = appendValue(arr, n, value);
  if (arr == NULL) {
    printf("확장 배열: 할당 실패\n");
    return 0;
  }

  printf("확장 배열:");
  for (i = 0; i < n + 1; i++) printf(" %d", arr[i]);
  printf("\n");
  free(arr);
  return 0;
}

int* appendValue(int* arr, int oldSize, int value) {
  // Todo: realloc으로 배열 크기를 oldSize+1로 늘리고 마지막 칸에 value를 저장한 뒤 새 주소를 반환하세요.
  return arr;
}
