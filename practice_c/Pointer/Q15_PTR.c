#include <stdio.h>

/* 포인터 연습 문제 15
문제: 삼중 포인터를 사용해 2차원 행렬의 특정 원소 값을 읽으세요.
난이도: 삼중 포인터 응용
입력: rows cols, 이어서 rows x cols 정수, 마지막에 row col
출력: 선택한 값: x
*/

int valueAt(int*** matrixRef, int row, int col);

int main(void) {
  int rows, cols;
  int storage[10][10];
  int* matrix[10];
  int** matrixPtr = matrix;
  int r, c;
  int i, j;

  if (scanf("%d %d", &rows, &cols) != 2) return 0;
  for (i = 0; i < rows; i++) {
    matrix[i] = storage[i];
    for (j = 0; j < cols; j++) scanf("%d", &storage[i][j]);
  }
  if (scanf("%d %d", &r, &c) != 2) return 0;

  printf("선택한 값: %d\n", valueAt(&matrixPtr, r, c));
  return 0;
}

int valueAt(int*** matrixRef, int row, int col) {
  // Todo: 삼중 포인터를 한 단계씩 역참조해 row행 col열 값을 반환하세요.
  // 즉 matrixRef -> int** -> int* -> int 순서로 접근합니다.
  return 0;
}
