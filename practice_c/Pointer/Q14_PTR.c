#include <stdio.h>
#include <string.h>

/* 포인터 연습 문제 14
문제: 문자열 포인터 배열을 가리키는 cursor를 이동시키며, 길이가 기준보다 긴 다음 단어를 찾으세요.
난이도: 삼중 포인터 입문
입력: n, 최소 길이, 이어서 n개 단어
출력: 조건을 만족하는 단어를 순서대로 출력
*/

char* nextLongWord(char*** cursor, int* remaining, int minLen);

int main(void) {
  int n;
  int minLen;
  char storage[20][101];
  char* words[20];
  char** cursor;
  char* found;
  int i;
  int remaining;
  int count = 0;

  if (scanf("%d %d", &n, &minLen) != 2) return 0;
  for (i = 0; i < n; i++) {
    scanf("%100s", storage[i]);
    words[i] = storage[i];
  }

  cursor = words;
  remaining = n;
  while ((found = nextLongWord(&cursor, &remaining, minLen)) != NULL) {
    printf("긴 단어 %d: %s\n", ++count, found);
  }

  if (count == 0) printf("긴 단어: 없음\n");
  return 0;
}

char* nextLongWord(char*** cursor, int* remaining, int minLen) {
  // Todo: *cursor는 현재 char* 배열의 현재 위치를 가리킵니다.
  // 길이가 minLen보다 큰 단어를 찾을 때까지 cursor와 remaining을 함께 갱신하고,
  // 찾으면 그 단어를 반환하세요. 더 이상 없으면 NULL을 반환하세요.
  return NULL;
}
