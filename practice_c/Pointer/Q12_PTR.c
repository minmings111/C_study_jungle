#include <stdio.h>
#include <string.h>

/* 포인터 연습 문제 12
문제: 문자열 포인터 배열에서 가장 긴 단어의 인덱스를 찾으세요.
난이도: 포인터 배열
입력: n, 이어서 n개 단어
출력: 가장 긴 단어 인덱스: i / 가장 긴 단어: word
*/

int indexOfLongestWord(char* words[], int n);

int main(void) {
  int n;
  char storage[20][101];
  char* words[20];
  int i;
  int index;

  if (scanf("%d", &n) != 1) return 0;
  for (i = 0; i < n; i++) {
    scanf("%100s", storage[i]);
    words[i] = storage[i];
  }

  index = indexOfLongestWord(words, n);
  if (index < 0) {
    printf("가장 긴 단어 인덱스: -1\n");
    printf("가장 긴 단어: 없음\n");
  } else {
    printf("가장 긴 단어 인덱스: %d\n", index);
    printf("가장 긴 단어: %s\n", words[index]);
  }
  return 0;
}

int indexOfLongestWord(char* words[], int n) {
  // Todo: char* 포인터 배열을 순회하며 가장 긴 문자열의 인덱스를 반환하세요.
  // 길이가 같으면 먼저 나온 단어의 인덱스를 유지하세요.
  return -1;
}
