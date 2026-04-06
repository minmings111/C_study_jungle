#include <stdio.h>

/* 포인터 연습 문제 6
문제: 두 배열을 비교해 처음으로 값이 다른 인덱스를 찾으세요.
입력: n, 배열 A, 배열 B
출력: 첫 불일치 인덱스: i
*/

int firstMismatchIndex(const int* a, const int* b, int n);

int main(void) {
  int n;
  int a[100];
  int b[100];
  int i;

  if (scanf("%d", &n) != 1) return 0;
  for (i = 0; i < n; i++) scanf("%d", &a[i]);
  for (i = 0; i < n; i++) scanf("%d", &b[i]);

  printf("첫 불일치 인덱스: %d\n", firstMismatchIndex(a, b, n));
  return 0;
}

int firstMismatchIndex(const int* a, const int* b, int n) {
  // Todo: 두 포인터를 함께 이동시키며 처음 값이 다른 위치의 인덱스를 반환하세요. 모두 같으면 -1을 반환하세요.

  for(int i =0; i<n; i++){
    if(a[i] != b[i]){
      return i;
    }
  }
  return -1;
}
