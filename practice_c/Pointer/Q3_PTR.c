#include <stdio.h>

/* 포인터 연습 문제 3
문제: 배열에서 최댓값 원소를 가리키는 포인터를 반환하세요.
입력: n, 이어서 n개 정수
출력: 최댓값: v / 인덱스: i
*/

int* findMaxPtr(int* arr, int n);

/*
짧은 이해 포인트
- 이 문제는 최댓값 자체가 아니라 "최댓값 원소의 주소"를 반환하는 문제입니다.
- maxPtr이 &arr[3]이라면, *maxPtr은 최댓값이고 maxPtr - arr는 인덱스 3입니다.
- 진짜 포인터 순회 스타일은 int* ptr = arr; 로 시작해서 ptr++ 하며 *ptr 값을 비교하는 방식입니다.
*/

int main(void) {
  int n;
  int arr[100];
  int* maxPtr;
  int i;

  if (scanf("%d", &n) != 1) return 0;
  for (i = 0; i < n; i++) scanf("%d", &arr[i]);

  maxPtr = findMaxPtr(arr, n);
  if (maxPtr == NULL) {
    printf("최댓값: 없음\n");
    printf("인덱스: -1\n");
  } else {
    printf("최댓값: %d\n", *maxPtr);
    printf("인덱스: %ld\n", (long)(maxPtr - arr));
  }
  return 0;
}

int* findMaxPtr(int* arr, int n) {
  // Todo: 포인터 순회로 최댓값 원소의 주소를 찾아 반환하세요. n이 0 이하이면
  // NULL을 반환하세요.
  if (n <= 0) return NULL;

  int curr = arr[0];
  int* currPtr = &arr[0];
  // 현재 풀이는 인덱스 방식입니다.
  // arr[i]는 i번째 원소 값, &arr[i]는 그 원소의 주소입니다.
  // 예: arr = [3, 8, 2, 9, 4] 이면 currPtr은 처음에 &arr[0]을 가리킵니다.

  for (int i = 1; i < n; ++i) {
    // arr[i]가 더 크면 최댓값과 최댓값 주소를 함께 갱신
    if (curr < arr[i]) {
      curr = arr[i];
      currPtr = &arr[i];
    }
  }

  // 포인터 순회 스타일로 쓰면 개념상 아래와 같습니다.
  // int* ptr = arr;
  // int* maxPtr = arr;
  // while (ptr < arr + n) {
  //   if (*maxPtr < *ptr) maxPtr = ptr;
  //   ptr++;
  // }
  // return maxPtr;
  return currPtr;
}
