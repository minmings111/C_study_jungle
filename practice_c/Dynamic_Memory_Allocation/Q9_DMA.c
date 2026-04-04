#include <stdio.h>
#include <stdlib.h>

/* 동적 할당 연습 문제 9
문제: 입력 단어들을 각각 동적으로 복제해 문자열 배열에 저장하세요.
입력: n, 이어서 n개 단어
출력: 복제 단어들: ...
*/

char** cloneWords(char words[][101], int n);
void freeWords(char** words, int n);

int main(void) {
  int n;
  char words[20][101];
  char** cloned;
  int i;

  if (scanf("%d", &n) != 1) return 0;
  for (i = 0; i < n; i++) scanf("%100s", words[i]);

  cloned = cloneWords(words, n);
  if (cloned == NULL) {
    printf("복제 단어들: 할당 실패\n");
    return 0;
  }

  printf("복제 단어들:");
  for (i = 0; i < n; i++) printf(" %s", cloned[i]);
  printf("\n");

  freeWords(cloned, n);
  return 0;
}

char** cloneWords(char words[][101], int n) {
  // Todo: char* 포인터 배열을 할당한 뒤, 각 단어를 개별적으로 동적 복제해 저장하세요.
  return NULL;
}

void freeWords(char** words, int n) {
  int i;
  if (words == NULL) return;
  for (i = 0; i < n; i++) free(words[i]);
  free(words);
}
