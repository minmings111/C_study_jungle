#include <stdio.h>
#include <stdlib.h>

/* 동적 할당 연습 문제 3
문제: 입력 배열을 힙 메모리에 복제하세요.
입력: n, 이어서 n개 정수
출력: 복제 배열: ...
*/

int* cloneArray(const int* src, int n);

int main(void) {
  int n;
  int src[100];
  int* copy;
  int i;

  if (scanf("%d", &n) != 1) return 0;
  for (i = 0; i < n; i++) scanf("%d", &src[i]);

  copy = cloneArray(src, n);
  if (copy == NULL) {
    printf("복제 배열: 할당 실패\n");
    return 0;
  }

  printf("복제 배열:");
  for (i = 0; i < n; i++) printf(" %d", copy[i]);
  printf("\n");
  free(copy);
  return 0;
}

int* cloneArray(const int* src, int n) {
  // Todo: malloc으로 새 배열을 만들고 src의 값을 모두 복사해 반환하세요.
  return NULL;
}
