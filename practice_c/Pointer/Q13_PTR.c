#include <stdio.h>

/* 포인터 연습 문제 13
문제: int* 행 포인터 배열로 표현된 정사각 행렬의 주대각선 합을 구하세요.
난이도: 이중 포인터 중급
입력: n, 이어서 n x n 정수
출력: 주대각선 합: x
*/

int sumMainDiagonal(int** matrix, int n);

int main(void) {
  int n;
  int storage[10][10];
  int* matrix[10];
  int i, j;

  if (scanf("%d", &n) != 1) return 0;
  for (i = 0; i < n; i++) {
    matrix[i] = storage[i];
    for (j = 0; j < n; j++) scanf("%d", &storage[i][j]);
  }

  printf("주대각선 합: %d\n", sumMainDiagonal(matrix, n));
  return 0;
}

int sumMainDiagonal(int** matrix, int n) {
  // Todo: int** matrix에서 matrix[i][i] 값을 모두 더해 주대각선 합을 반환하세요.
  return 0;
}
