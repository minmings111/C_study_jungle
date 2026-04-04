#include <stdio.h>
#include <stdlib.h>

/* 동적 할당 연습 문제 7
문제: rows x cols 크기의 2차원 행렬을 동적으로 만들고 입력값을 저장한 뒤, 각 행의 합을 출력하세요.
입력: rows cols, 이어서 rows*cols개 정수
출력: 각 행의 합
*/

int** allocateMatrix(int rows, int cols);
void freeMatrix(int** matrix, int rows);

int main(void) {
  int rows, cols;
  int** matrix;
  int i, j;
  int rowSum;

  if (scanf("%d %d", &rows, &cols) != 2) return 0;
  matrix = allocateMatrix(rows, cols);
  if (matrix == NULL) {
    printf("행렬 할당 실패\n");
    return 0;
  }

  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) scanf("%d", &matrix[i][j]);
  }

  for (i = 0; i < rows; i++) {
    rowSum = 0;
    for (j = 0; j < cols; j++) rowSum += matrix[i][j];
    printf("행 %d 합: %d\n", i, rowSum);
  }

  freeMatrix(matrix, rows);
  return 0;
}

int** allocateMatrix(int rows, int cols) {
  // Todo: int* 행 포인터 배열과 각 행의 int 배열을 차례로 할당해 2차원 행렬을 구성하세요.
  return NULL;
}

void freeMatrix(int** matrix, int rows) {
  int i;
  if (matrix == NULL) return;
  for (i = 0; i < rows; i++) free(matrix[i]);
  free(matrix);
}
