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
/* 이 코드에서는 메모리 크기와 주소를 8바이트 단위로 맞추겠다는 뜻이다. */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
/* ALIGNMENT의 배수 중 가장 가까운 위쪽 값으로 올림 */
/* 예: ALIGN(1)=8, ALIGN(8)=8, ALIGN(9)=16 */
/* 이렇게 맞추는 이유는 malloc이 반환하는 주소와 블록 크기를
   정렬 규칙에 맞게 유지하기 위해서이다. */
/* ~0x7은 비트로 보면 마지막 3비트를 0으로 만든 마스크이다.
   8의 배수는 이진수에서 끝 3비트가 항상 000이므로,
   & ~0x7 연산을 하면 8의 배수 형태로 맞출 수 있다. 
   +(ALIGNMENT-1)를 하지 않고 ~0x7를 하면 올림이 아니라 내림이 됨*/
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

// 0x111

/* size_t는 "크기"를 나타낼 때 쓰는 정수 타입이다.
   예를 들어 malloc의 요청 크기, 배열 길이, 메모리 크기 같은 값을 담는다. */
/* sizeof(size_t)는 size_t 타입이 메모리에서 몇 바이트를 차지하는지 뜻한다. */
/* SIZE_T_SIZE는 size_t 하나를 저장할 공간 크기를 정렬까지 고려해서 구한 값이다.
   이 코드에서는 블록 맨 앞에 요청 크기(size)를 저장하기 위한 칸 크기로 사용된다. */
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

// 기본 워드 사이즈
#define WSIZE (SIZE_T_SIZE) //32비트는 4, 64비트는 8
#define DSIZE (SIZE_T_SIZE * 2)

// 힙 확장 요청 시 기본으로 늘릴 사이즈 <= 힙 확장을 너무 자주 하면 비효율적
// 메모리 페이지 크기가 보통 4KB(= 4096 byte), 힙 확장 시 처음부터 페이지 관리 단위로 생성해버리자
#define CHUNKSIZE (1<<12) // 4096 byte
// 요청값이 기본 사이즈보다 크면 요청 사이즈만큼, 요청값이 기본 사이즈보다 작으면 기본 사이즈만큼 힙 확장
#define MAX(x, y) ((x) > (y) ? (x) : (y)) 

// 블록 크기와 할당 여부 비트를 하나의 정수값으로 합치는 매크로
#define PACK(size, alloc) ((size) | (alloc))

// 어떤 주소 p를 size_t 한 칸이 저장된 위치로 보고, 그 위치의 값을 읽기/쓰기
#define GET(p) (*(size_t *)(p))
#define PUT(p, val) (*(size_t *)(p) = (val))

// 포인터 p가 가리키는 헤더나 풋터로부터 이미 저장된 블록의 크기 정보를 추출
#define GET_SIZE(p) (GET(p) & ~0x7)
//포인터 p가 가리키는 헤더나 풋터로부터 이미 저장된 할당 비트 정보를 추출
#define GET_ALLOC(p) (GET(p) & 0x1)

// bp가 가리키는 payload 시작 주소로부터 header 위치를 계산
#define HDRP(bp) ((char *)(bp) - WSIZE)
// bp가 가리키는 payload 시작 주소로부터 footer 위치를 계산
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

// bp의 다음 블록 주소 계산
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
// bp의 이전 블록 주소 계산
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))


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

// implicit free list
void *mm_malloc(size_t size)
{
   /* 요청한 크기 size에 관리용 크기 저장 공간(SIZE_T_SIZE)을 더하고,
      ALIGN 단위에 맞춰 실제로 확보할 총 블록 크기를 계산한다. */
   int newsize = ALIGN(size + SIZE_T_SIZE);

   /* mem_sbrk로 힙을 newsize만큼 늘리고,
      새로 확보된 블록의 시작 주소를 p에 저장한다. */
   void *p = mem_sbrk(newsize); 

   /* mem_sbrk가 실패하면 (void *)-1을 반환하므로,
      malloc 실패로 보고 NULL을 반환한다. */
   if (p == (void *)-1) {
      return NULL;
   } else {
      /* 블록의 맨 앞 주소 p 위치에
         사용자가 원래 요청한 크기 size를 기록해 둔다. */
      *(size_t *)p = size;

      /* 블록 맨 앞에는 size 정보가 들어 있으므로
         그 칸을 건너뛴 payload 시작 주소를 사용자에게 반환한다. */
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
   if (newptr == NULL) {
      return NULL;
   }
   copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
   if (size < copySize) {
      copySize = size;
   }
   memcpy(newptr, oldptr, copySize);
   mm_free(oldptr);
   return newptr;
}
