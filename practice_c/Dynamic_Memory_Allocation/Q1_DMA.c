#include <stdio.h>
#include <stdlib.h>

/* 동적 할당 연습 문제 1
문제: 정수 하나를 동적으로 할당하고 값을 저장하세요.
입력: 정수 1개
출력: 동적 값: x
*/

int* allocateInt(int value);

int main(void) {
  int value;
  int* ptr;

  if (scanf("%d", &value) != 1) return 0;

  ptr = allocateInt(value);
  if (ptr == NULL) {
    printf("동적 값: 할당 실패\n");
    return 0;
  }

  printf("동적 값: %d\n", *ptr);
  free(ptr); // 여기서 메모리 해제
  return 0;
}

int* allocateInt(int value) {
  // Todo: int 1개를 malloc으로 할당하고 value를 저장한 뒤 그 주소를 반환하세요.
  int* temp = (int*)malloc(sizeof(int));

  if (temp == NULL) {
    return NULL;
  }

  *temp = value;

  return temp;
}
