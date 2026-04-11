#include <stdio.h>

extern int mm_init (void);
extern void *mm_malloc (size_t size);
extern void mm_free (void *ptr);
extern void *mm_realloc(void *ptr, size_t size);


/* 
 * Students work in teams of one or two.  Teams enter their team name, 
 * personal names and login IDs in a struct of this
 * type in their bits.c file.
 */
/*
 * 학생들은 1명 또는 2명 팀으로 작업합니다.
 * 팀은 이 형태의 구조체에 팀 이름, 개인 이름,
 * 로그인 ID를 bits.c 파일에 입력합니다.
 */
typedef struct {
    char *teamname; /* ID1+ID2 or ID1 */
                   /* ID1+ID2 또는 ID1 */
    char *name1;    /* full name of first member */
                   /* 첫 번째 팀원의 전체 이름 */
    char *id1;      /* login ID of first member */
                   /* 첫 번째 팀원의 로그인 ID */
    char *name2;    /* full name of second member (if any) */
                   /* 두 번째 팀원의 전체 이름 (있다면) */
    char *id2;      /* login ID of second member */
                   /* 두 번째 팀원의 로그인 ID */
} team_t;

extern team_t team;
