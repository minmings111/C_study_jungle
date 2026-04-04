#include <stdio.h>
#include <stdlib.h>

/* 동적 할당 연습 문제 8
문제: 입력 행렬의 전치 행렬을 새 동적 메모리에 만들어 출력하세요.
입력: rows cols, 이어서 rows*cols개 정수
출력: 전치 행렬
*/

int** allocateMatrix(int rows, int cols);
int** transposeMatrix(int** matrix, int rows, int cols);
void freeMatrix(int** matrix, int rows);

int main(void) {
  int rows, cols;
  int** matrix;
  int** transposed;
  int i, j;

  if (scanf("%d %d", &rows, &cols) != 2) return 0;
  matrix = allocateMatrix(rows, cols);
  if (matrix == NULL) return 0;
  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) scanf("%d", &matrix[i][j]);
  }

  transposed = transposeMatrix(matrix, rows, cols);
  if (transposed == NULL) {
    freeMatrix(matrix, rows);
    printf("전치 행렬 할당 실패\n");
    return 0;
  }

  for (i = 0; i < cols; i++) {
    printf("행 %d:", i);
    for (j = 0; j < rows; j++) printf(" %d", transposed[i][j]);
    printf("\n");
  }

  freeMatrix(matrix, rows);
  freeMatrix(transposed, cols);
  return 0;
}

int** allocateMatrix(int rows, int cols) {
  // Todo: rows x cols 크기의 2차원 int 행렬을 동적으로 할당하세요.
  return NULL;
}

int** transposeMatrix(int** matrix, int rows, int cols) {
  // Todo: cols x rows 크기의 새 행렬을 할당하고 matrix[j][i]를 새 위치에 복사하세요.
  return NULL;
}

void freeMatrix(int** matrix, int rows) {
  int i;
  if (matrix == NULL) return;
  for (i = 0; i < rows; i++) free(matrix[i]);
  free(matrix);
}
