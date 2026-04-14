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
team_t team = {
   /* Team name */
   "ateam",
   /* First member's full name */
   "Harry Bovik",
   /* First member's email address */
   "bovik@cs.cmu.edu",
   /* Second member's full name (leave blank if none) */
   "",
   /* Second member's email address (leave blank if none) */
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
#define WSIZE (SIZE_T_SIZE)
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


// 변수 및 함수 원형 선언
static char * heap_listp;
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void * find_fit(size_t asize);
static void place(void *bp, size_t asize);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
   // mem_sbrk(4*WSIZE)로 초기 힙 구조 4칸 분량의 공간을 확보한다.
   // 1칸:alignment padding, 2:prologue header, 3:prologue footer, 4:epilogue header
   // mem_sbrk가 실패하면 (void *)-1을 반환하므로 초기화 실패로 -1을 반환
   // 공간확보와 엣지 케이스를 동시에 수행하면 기분이 좋음!
   if((heap_listp = mem_sbrk(4*WSIZE)) == (void*) -1){
      return -1;
   }

   // padding은 실제 블록이 아니며 값 0으로 초기화
   PUT(heap_listp, 0); // 정렬을 맞추기 위한 alignment padding
   // PACK(DSIZE, 1)은 "블록 크기 DSIZE, 할당 상태 1"을 한 값으로 합친 것이다.
   PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); // prologue header
   // 프롤로그는 가짜 allocated block처럼 보이게 해야 하므로 헤더와 같은 값을 넣는다.
   PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); // prologue footer
   // 에필로그는 힙 끝 표시용 특수 블록이므로 크기 0, 할당 상태 1로 둔다.
   PUT(heap_listp + (3*WSIZE), PACK(0, 1)); // eplilogue header

   // heap_listp는 이후 힙을 순회할 때 사용할 기준 포인터
   // (지금은 payload가 없으므로)prologue footer를 가리키도록 2칸 앞으로 이동
   heap_listp += (2*WSIZE);

   // 초기 힙 뼈대만 만든 상태이므로, 실제로 사용할 수 있는 free block을 미리 하나 만들어두면 좋지용~
   // extend_heap이 실패하면 초기화 실패로 -1을 반환한다.
   if(extend_heap(CHUNKSIZE/WSIZE) == NULL){
      return -1;
   }

   // 여기까지 오면 초기 힙 구조 생성이 성공한 것이므로 0을 반환한다.
   return 0;
}
/*
mem_sbrk는 진짜로 힙 공간만을 늘리는 저수준 함수이고,
extend_heap은 그걸 감싸서 allocator가 쓰기 좋은 형태로 힙 확장을 정리하는 헬퍼 함수
새로 늘어난 공간을 free block으로 세팅하고,epilogue 갱신해줌. 필요하면 coalesce도 수행
*/
static void *extend_heap(size_t words){
   // bp는 새로 확장한 free block의 payload 시작 주소로 사용할 포인터이다.
   char *bp;
   // size는 실제로 힙을 몇 바이트 확장할지 저장하는 변수이다.
   size_t size;

   // words 개수를 실제 바이트 수로 바꾼다.
   // words가 홀수이면 한 워드를 더해서 짝수 워드 크기로 맞춘다.
   // 이렇게 해서 블록 크기가 정렬 규칙을 만족하도록 한다.
   size = (words%2) ? (words+1) * WSIZE : words * WSIZE;

   // mem_sbrk로 힙을 size만큼 늘리고, 새로 확보한 영역의 시작 주소를 bp에 저장한다.
   // 실패하면 NULL을 반환한다.
   if((bp = mem_sbrk(size)) == (void *)-1){
      return NULL;
   }

   // 새로 확보한 공간을 free block으로 사용하기 위해
   // 해당 블록의 헤더에 "크기 size, 할당 상태 0"을 기록한다.
   PUT(HDRP(bp), PACK(size, 0)); // free block header

   // 같은 free block의 푸터에도 같은 값을 기록한다.
   PUT(FTRP(bp), PACK(size, 0)); // free block footer

   // 새로 만들어진 free block 뒤에는 힙 끝 표시용 새 epilogue header를 다시 기록한다.
   PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); // new epilogue header

   // 앞 블록과 지금 블록이 둘 다 free라면 하나로 합칠 수 있으므로
   // coalesce를 호출한 뒤 그 결과 블록 포인터를 반환한다.
   return coalesce(bp);
}

// free block은 합쳐둘 수 있다면 합쳐야 함.
static void *coalesce(void *bp){
   // 현재 블록(bp) 바로 앞 블록의 footer를 보고, 이전 블록이 할당 상태인지 확인
   size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
   size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
   // 현재 블록(bp)의 header에서 블록 전체 크기를 가져옴
   size_t size = GET_SIZE(HDRP(bp));

   // case1 : 앞, 뒤 블록 모두 사용 불가
   if(prev_alloc && next_alloc){
      return bp;
   }

   // case2 : 앞 블록 사용불가, 뒤 블록 사용 가능
   else if (prev_alloc && !next_alloc){
      // 다음 블록의 크기를 현재 size에 더함
      size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
      PUT(HDRP(bp), PACK(size, 0));
      PUT(FTRP(bp), PACK(size, 0));
   }
   // case3 : 앞 블록 사 가능, 뒤 블록 사용 불가
   else if (!prev_alloc && next_alloc){
      size += GET_SIZE(HDRP(PREV_BLKP(bp)));
      PUT(FTRP(bp), PACK(size, 0));
      PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
      bp = PREV_BLKP(bp);
   }
   else{//case 4 : 앞, 뒤 블록 모두 사용 가능
      size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
               GET_SIZE(FTRP(NEXT_BLKP(bp)));
      PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
      PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
      bp = PREV_BLKP(bp);
   }

   return bp;

}

void * find_fit(size_t asize){
   void* bp = heap_listp;
   while(GET_SIZE(HDRP(bp)) != 0){ // 에필로그 블록(사이즈 0)을 만나면 중단
      // 크기 검토, 사용가능 여부 검토
      if(asize <= GET_SIZE(HDRP(bp)) && GET_ALLOC(HDRP(bp)) == 0){
         return bp;
      }
      else{
         bp = NEXT_BLKP(bp); // 다음 블록으로
      }
   }
   return NULL; // 프리 블록 없음
}



/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
/*
 * mm_malloc - brk 포인터를 증가시켜 블록을 할당한다.
 *             항상 정렬 단위의 배수 크기로 블록을 할당해야 한다.
*/
void *mm_malloc(size_t size){
   //할당 요청 크기가 0이면 바로 종료
   if(size == 0){
      return NULL;
   }
   //asize: size를 align(정렬한) 크기
   size_t asize; 

   //요구하는 페이로드 크기(size)가 최소 단위(DSIZE)보다 작을 때 
   if(size <= DSIZE)
      asize = 2*DSIZE; //헤더푸터DSIZE + 페이로드 최소 크기 DSIZE
    //size가 최소단위
   else
      // (size + DSIZE + (DSIZE-1))
      asize = ALIGN(size + DSIZE);

   // free block 중 재활용 가능한 블록이 있는지 검토
   void * p = find_fit(asize);
   if(p == NULL){ // 재활용 못 하면 힙 추가
      if ((p = extend_heap(asize/WSIZE)) == NULL) {
         return NULL;
      }

   }
   // 사용 블록 확정, 남은 공간 처리
   place(p, asize);
   // 사용자에게 사용할 블록 주소 넘기기
   return p;
}


// free block에 배치(필요하면 분할!!)
static void place(void *bp, size_t asize){ // asize는 조정된 블록 크기
   // csize는 현재 작업할 대상인 free block의 크기
   size_t csize = GET_SIZE(HDRP(bp));

   // 여유공간이 많아서 분할하는 경우
   if((csize - asize) >= (2 * DSIZE)){
      // 사용자에게 줄 블럭
      PUT(HDRP(bp), PACK(asize, 1));
      PUT(FTRP(bp), PACK(asize, 1));
      
      // 다음 free block
      bp = NEXT_BLKP(bp);
      PUT(HDRP(bp), PACK(csize-asize, 0));
      PUT(FTRP(bp), PACK(csize-asize, 0));
   }
   else{ // 여유공간이 부족해서 내부 단편화 하는 경우(분할X)
      PUT(HDRP(bp), PACK(csize, 1));
      PUT(FTRP(bp), PACK(csize, 1));
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
   // NULL 포인터가 들어오면 해제할 대상이 없으므로 종료
   if(ptr == NULL){
      return;
   }

   // 현재 블록의 헤더에서 블록 크기를 가져옴
   size_t size = GET_SIZE(HDRP(ptr));

   // 헤더에 현재 크기와 할당 비트 0을 기록해 free 블록으로 표시
   PUT(HDRP(ptr), PACK(size, 0));
   PUT(FTRP(ptr), PACK(size, 0));
   coalesce(ptr); // 인접한 free 블록이 있으면 병합
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
/*
 * mm_realloc - mm_malloc과 mm_free를 이용해 단순하게 구현한다.
 */
void *mm_realloc(void *ptr, size_t size)
{  
   // ptr이 NULL이면 malloc처럼 처리
   if(ptr == NULL) 
      return mm_malloc(size);
   // size가 0이면 free 후 NULL 반환
   if(size == 0){ 
      mm_free(ptr);
      return NULL;
   }

   void *oldptr = ptr; // 기존 블록
   void *newptr; // 새 블록
   size_t copySize; // 복사할 크기

   newptr = mm_malloc(size); // 새 블록 할당
   // 할당 실패 시 종료
   if (newptr == NULL)
        return NULL;

   // 기존 payload 크기 계산
   copySize = GET_SIZE(HDRP(oldptr)) - DSIZE;
   // 더 작은 크기만 복사
   if (size < copySize)
        copySize = size;

   memcpy(newptr, oldptr, copySize); // 데이터 복사
   mm_free(oldptr); // 기존 블록 해제
   return newptr; // 새 포인터 반환
}
