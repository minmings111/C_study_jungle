#include <stdio.h>

/* 포인터 연습 문제 4
문제: 배열을 제자리에서 뒤집으세요.
입력: n, 이어서 n개 정수
출력: 뒤집은 배열: ...
*/

void reverseArray(int* arr, int n);

/*
짧은 이해 포인트
- 배열 이름 arr는 식 안에서 보통 첫 원소 주소인 &arr[0]처럼 동작합니다.
- 그래서 arr + k 는 k번째 원소의 주소를 뜻하고, arr[k]는 *(arr + k)와 같습니다.
- 이건 연산자 오버라이딩이 아니라 C의 포인터 산술 문법입니다.
- 일반 변수 하나의 주소에도 p + 1 같은 계산은 문법상 가능하지만, 배열처럼 연속된 메모리가 아니면 안전하게 쓰면 안 됩니다.
*/

int main(void) {
  int n;
  int arr[100];
  int i;

  if (scanf("%d", &n) != 1) return 0;
  for (i = 0; i < n; i++) scanf("%d", &arr[i]);

  reverseArray(arr, n);

  printf("뒤집은 배열:");
  for (i = 0; i < n; i++) printf(" %d", arr[i]);
  printf("\n");
  return 0;
}

void reverseArray(int* arr, int n) {
  // Todo: left와 right 포인터를 이용해 양 끝 값을 교환하면서 배열을 뒤집으세요.

  int* left = arr;
  // arr는 첫 원소 주소처럼 동작하므로 left는 &arr[0]과 같습니다.
  int* right = arr + n - 1;
  // arr + n - 1 은 마지막 원소 주소, 즉 &arr[n - 1] 입니다.
  // 예: arr = [1, 2, 3, 4, 5] 이고 n = 5면 right는 5를 가리킵니다.
  // left++, right-- 는 바이트 1칸이 아니라 int 원소 1칸씩 이동합니다.
  int* temp;

  while (left != right) {
    temp = *left;
    *left = *right;
    *right = temp;
    left++;
    right--;
  }
}
