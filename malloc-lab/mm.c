/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
/*
 * mm-naive.c - 가장 빠르지만 메모리 효율은 가장 낮은 malloc 패키지입니다.
 *
 * 이 단순한 접근에서는 brk 포인터를 단순히 증가시켜 블록을 할당합니다.
 * 각 블록은 순수한 payload만 가지며, 헤더나 푸터는 없습니다.
 * 블록은 병합(coalesce)되거나 재사용되지 않습니다.
 * realloc은 mm_malloc과 mm_free를 직접 이용해 구현됩니다.
 *
 * 학생 안내: 이 헤더 주석은 자신의 풀이를 높은 수준에서 설명하는
 * 주석으로 교체하세요.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
/*********************************************************
 * 학생 안내: 다른 작업을 하기 전에 아래 구조체에
 * 팀 정보를 먼저 입력하세요.
 ********************************************************/
team_t team = {
    /* Team name */
    /* 팀 이름 */
    "ateam",
    /* First member's full name */
    /* 첫 번째 팀원의 전체 이름 */
    "Harry Bovik",
    /* First member's email address */
    /* 첫 번째 팀원의 이메일 주소 */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    /* 두 번째 팀원의 전체 이름 (없으면 빈칸) */
    "",
    /* Second member's email address (leave blank if none) */
    /* 두 번째 팀원의 이메일 주소 (없으면 빈칸) */
    ""
};

/* single word (4) or double word (8) alignment */
/* 단어 크기(4) 또는 더블 워드 크기(8) 정렬 */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
/* ALIGNMENT의 배수 중 가장 가까운 위쪽 값으로 올림 */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* 
 * mm_init - initialize the malloc package.
 */
/*
 * mm_init - malloc 패키지를 초기화한다.
 */
int mm_init(void)
{
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
/*
 * mm_malloc - brk 포인터를 증가시켜 블록을 할당한다.
 *             항상 정렬 단위의 배수 크기로 블록을 할당해야 한다.
 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
/*
 * mm_free - 블록을 해제해도 아무 동작도 하지 않는다.
 */
void mm_free(void *ptr)
{
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
/*
 * mm_realloc - mm_malloc과 mm_free를 이용해 단순하게 구현한다.
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}













