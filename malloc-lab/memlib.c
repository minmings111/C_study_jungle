/*
 * memlib.c - a module that simulates the memory system.  Needed because it 
 *            allows us to interleave calls from the student's malloc package 
 *            with the system's malloc package in libc.
 */
/*
 * memlib.c - 메모리 시스템을 흉내 내는 모듈입니다.
 *            학생의 malloc 패키지 호출과 libc의 시스템 malloc 호출을
 *            함께 다룰 수 있도록 필요합니다.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

#include "memlib.h"
#include "config.h"

/* private variables */
/* 내부에서만 사용하는 변수들 */
/* 이 파일 밖에서는 직접 접근하지 않고, memlib 함수들을 통해서만 간접적으로 사용한다. */
static char *mem_start_brk;  /* points to first byte of heap */
                            /* 힙의 첫 번째 바이트를 가리킨다 */
                            /* malloc(MAX_HEAP)으로 확보한 큰 메모리 공간의 시작 주소이다. */
static char *mem_brk;        /* points to last byte of heap */
                            /* 힙의 마지막 바이트를 가리킨다 */
                            /* 현재까지 사용 중인 힙의 끝 경계를 나타낸다.
                               새 메모리를 할당하면 이 값이 뒤로 이동한다. */
static char *mem_max_addr;   /* largest legal heap address */ 
                            /* 허용되는 가장 큰 힙 주소 */
                            /* 힙이 이 주소를 넘어가면 더 이상 메모리를 늘릴 수 없다. */

/* 
 * mem_init - initialize the memory system model
 */
/*
 * mem_init - 메모리 시스템 모델을 초기화한다.
 */
void mem_init(void)
{
    /* 이 함수는 입력값을 받지 않는다.
       대신 전역(static) 포인터 3개를 초기 상태로 세팅한다. */

    /* allocate the storage we will use to model the available VM */
    /* 사용 가능한 가상 메모리를 흉내 내기 위한 저장 공간을 할당한다 */
    if ((mem_start_brk = (char *)malloc(MAX_HEAP)) == NULL) {
	fprintf(stderr, "mem_init_vm: malloc error\n");
	exit(1);
    }

    mem_max_addr = mem_start_brk + MAX_HEAP;  /* max legal heap address */
                                              /* 허용 가능한 최대 힙 주소 */
    mem_brk = mem_start_brk;                  /* heap is empty initially */
                                              /* 초기에는 힙이 비어 있다 */

    /* 정리:
       1. MAX_HEAP 크기만큼 큰 메모리 공간을 하나 확보한다.
       2. 그 시작 주소를 mem_start_brk에 저장한다.
       3. 끝 한계를 mem_max_addr에 저장한다.
       4. 아직 아무것도 할당되지 않았으므로 mem_brk도 시작점과 같게 둔다.

       반환값:
       - 없음(void)
       - 대신 전역 상태가 초기화된다. */
}

/* 
 * mem_deinit - free the storage used by the memory system model
 */
/*
 * mem_deinit - 메모리 시스템 모델이 사용한 저장 공간을 해제한다.
 */
void mem_deinit(void)
{
    /* 입력값은 없고, mem_init에서 malloc으로 확보했던 큰 메모리 공간을 반납한다.
       즉 "가짜 힙 전체"를 통째로 정리하는 함수라고 보면 된다. */
    free(mem_start_brk);

    /* 반환값:
       - 없음(void) */
}

/*
 * mem_reset_brk - reset the simulated brk pointer to make an empty heap
 */
/*
 * mem_reset_brk - 시뮬레이션된 brk 포인터를 초기화해서 빈 힙으로 만든다.
 */
void mem_reset_brk()
{
    /* 실제 메모리를 새로 malloc/free 하는 것이 아니라,
       "현재 힙의 끝" 위치만 처음으로 되돌린다.
       그래서 이미 확보된 큰 공간은 유지한 채, 비어 있는 힙처럼 다시 시작할 수 있다. */
    mem_brk = mem_start_brk;

    /* 처리 결과:
       - mem_brk가 시작 위치로 돌아감
       - 이후 mem_heapsize()는 0이 됨

       반환값:
       - 없음(void) */
}

/* 
 * mem_sbrk - simple model of the sbrk function. Extends the heap 
 *    by incr bytes and returns the start address of the new area. In
 *    this model, the heap cannot be shrunk.
 */
/*
 * mem_sbrk - sbrk 함수를 단순화해 모델링한 함수이다.
 *            힙을 incr 바이트만큼 늘리고 새 영역의 시작 주소를 반환한다.
 *            이 모델에서는 힙을 줄일 수 없다.
 */
void *mem_sbrk(int incr) 
{
    /* 입력값:
       - incr: 힙을 얼마나 늘릴지 나타내는 바이트 수

       예:
       - mem_brk가 100번지를 가리키고 incr가 24라면
         100~123 구간을 새로 할당한 것으로 보고,
         반환값은 기존 시작점인 100이 된다. */

    char *old_brk = mem_brk;
    /* old_brk는 "늘리기 전" 힙 끝 위치를 저장한다.
       나중에 이 값을 반환해야 새로 할당된 블록의 시작 주소가 된다. */

    if ( (incr < 0) || ((mem_brk + incr) > mem_max_addr)) {
	/* 두 가지 경우는 실패 처리한다.
	   1. incr가 음수인 경우: 이 모델에서는 힙 축소를 허용하지 않음
	   2. 늘린 뒤 위치가 최대 허용 주소를 넘는 경우: 메모리 부족 */
	errno = ENOMEM;
	fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory...\n");
	return (void *)-1;
    }

    /* 여기까지 왔으면 요청한 크기만큼 힙을 늘릴 수 있다는 뜻이다.
       따라서 현재 힙 끝 포인터를 incr만큼 뒤로 이동시킨다. */
    mem_brk += incr;

    /* 반환값:
       - 성공: 늘리기 전 시작 주소(old_brk)
       - 실패: (void *)-1

       이 반환 방식은 실제 sbrk의 동작과 비슷하게 설계되어 있다. */
    return (void *)old_brk;
}

/*
 * mem_heap_lo - return address of the first heap byte
 */
/*
 * mem_heap_lo - 힙의 첫 번째 바이트 주소를 반환한다.
 */
void *mem_heap_lo()
{
    /* 입력값:
       - 없음

       처리:
       - mem_init에서 만들어 둔 힙 시작 주소를 그대로 반환한다.

       반환값:
       - 힙의 가장 낮은 주소 */
    return (void *)mem_start_brk;
}

/* 
 * mem_heap_hi - return address of last heap byte
 */
/*
 * mem_heap_hi - 힙의 마지막 바이트 주소를 반환한다.
 */
void *mem_heap_hi()
{
    /* 입력값:
       - 없음

       처리:
       - mem_brk는 "현재 힙의 끝 바로 다음 위치"처럼 움직인다.
       - 따라서 실제 마지막 사용 가능 바이트 주소는 mem_brk - 1 이다.

       반환값:
       - 힙의 가장 높은 유효 주소 */
    return (void *)(mem_brk - 1);
}

/*
 * mem_heapsize() - returns the heap size in bytes
 */
/*
 * mem_heapsize() - 힙 크기를 바이트 단위로 반환한다.
 */
size_t mem_heapsize() 
{
    /* 입력값:
       - 없음

       처리:
       - 현재 힙 끝 위치(mem_brk)에서 시작 위치(mem_start_brk)를 빼면
         지금까지 사용 중인 힙 크기를 구할 수 있다.

       반환값:
       - 현재 힙 크기(바이트 단위) */
    return (size_t)(mem_brk - mem_start_brk);
}

/*
 * mem_pagesize() - returns the page size of the system
 */
/*
 * mem_pagesize() - 시스템의 페이지 크기를 반환한다.
 */
size_t mem_pagesize()
{
    /* 입력값:
       - 없음

       처리:
       - 운영체제가 사용하는 메모리 페이지 크기를 getpagesize()로 읽어 온다.

       반환값:
       - 시스템 페이지 크기 */
    return (size_t)getpagesize();
}
