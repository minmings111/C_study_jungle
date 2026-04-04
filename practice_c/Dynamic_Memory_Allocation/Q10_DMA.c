#include <stdio.h>
#include <stdlib.h>

/* 동적 할당 연습 문제 10
문제: 입력 단어들을 사이에 공백 하나씩 넣어 새 동적 문자열로 이어 붙이세요.
입력: n, 이어서 n개 단어
출력: 이어 붙인 문자열: ...
*/

char* joinWords(char words[][101], int n);

int main(void) {
  int n;
  char words[20][101];
  char* joined;
  int i;

  if (scanf("%d", &n) != 1) return 0;
  for (i = 0; i < n; i++) scanf("%100s", words[i]);

  joined = joinWords(words, n);
  if (joined == NULL) {
    printf("이어 붙인 문자열: 할당 실패\n");
    return 0;
  }

  printf("이어 붙인 문자열: %s\n", joined);
  free(joined);
  return 0;
}

char* joinWords(char words[][101], int n) {
  // Todo: 필요한 전체 길이를 먼저 계산한 뒤, 그 길이만큼 메모리를 할당하고 단어와 공백을 차례로 복사하세요.
  return NULL;
}
