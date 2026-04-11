#ifndef __CONFIG_H_
#define __CONFIG_H_

/*
 * config.h - malloc lab configuration file
 *
 * Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
 * May not be used, modified, or copied without permission.
 */
/*
 * config.h - malloc lab 설정 파일
 *
 * Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
 * 허가 없이 사용, 수정, 복사할 수 없습니다.
 */

/*
 * This is the default path where the driver will look for the
 * default tracefiles. You can override it at runtime with the -t flag.
 */
/*
 * 드라이버가 기본 trace 파일을 찾을 때 사용하는 기본 경로입니다.
 * 실행 시 -t 옵션으로 이 경로를 덮어쓸 수 있습니다.
 */
#define TRACEDIR "./traces/"

/*
 * This is the list of default tracefiles in TRACEDIR that the driver
 * will use for testing. Modify this if you want to add or delete
 * traces from the driver's test suite. For example, if you don't want
 * your students to implement realloc, you can delete the last two
 * traces.
 */
/*
 * 이것은 TRACEDIR 안에서 드라이버가 테스트에 사용할 기본 trace 파일 목록입니다.
 * 학생들에게 realloc 구현을 요구하지 않으려면 마지막 두 trace를 삭제하는 식으로
 * 테스트 세트를 조정할 수 있습니다.
 */
#define DEFAULT_TRACEFILES \
  "amptjp-bal.rep",\
  "cccp-bal.rep",\
  "cp-decl-bal.rep",\
  "expr-bal.rep",\
  "coalescing-bal.rep",\
  "random-bal.rep",\
  "random2-bal.rep",\
  "binary-bal.rep",\
  "binary2-bal.rep",\
  "realloc-bal.rep",\
  "realloc2-bal.rep"

/*
 * This constant gives the estimated performance of the libc malloc
 * package using our traces on some reference system, typically the
 * same kind of system the students use. Its purpose is to cap the
 * contribution of throughput to the performance index. Once the
 * students surpass the AVG_LIBC_THRUPUT, they get no further benefit
 * to their score.  This deters students from building extremely fast,
 * but extremely stupid malloc packages.
 */
/*
 * 이 상수는 기준 시스템에서 libc malloc이 이 trace들로 보이는
 * 예상 성능을 의미합니다. throughput 점수의 상한을 정하기 위해 사용됩니다.
 * 학생 풀이가 AVG_LIBC_THRUPUT를 넘더라도 추가 점수 이득은 없습니다.
 * 지나치게 빠르지만 형편없는 malloc 구현을 막기 위한 장치입니다.
 */
#define AVG_LIBC_THRUPUT      600E3  /* 600 Kops/sec */

 /* 
  * This constant determines the contributions of space utilization
  * (UTIL_WEIGHT) and throughput (1 - UTIL_WEIGHT) to the performance
  * index.  
  */
 /*
  * 이 상수는 성능 지수에서 공간 활용도(UTIL_WEIGHT)와
  * 처리량(1 - UTIL_WEIGHT)이 각각 얼마나 반영되는지 정합니다.
  */
#define UTIL_WEIGHT .60

/* 
 * Alignment requirement in bytes (either 4 or 8) 
 */
/*
 * 정렬 요구 사항(바이트 단위, 4 또는 8)
 */
#define ALIGNMENT 8  

/* 
 * Maximum heap size in bytes 
 */
/*
 * 힙의 최대 크기(바이트 단위)
 */
#define MAX_HEAP (20*(1<<20))  /* 20 MB */

/*****************************************************************************
 * Set exactly one of these USE_xxx constants to "1" to select a timing method
 *****************************************************************************/
/*****************************************************************************
 * 타이밍 방법을 선택하려면 아래 USE_xxx 상수 중 정확히 하나만 1로 설정하세요.
 *****************************************************************************/
#define USE_FCYC   0   /* cycle counter w/K-best scheme (x86 & Alpha only) */
                      /* K-best 방식을 사용하는 사이클 카운터 (x86, Alpha 전용) */
#define USE_ITIMER 0   /* interval timer (any Unix box) */
                      /* interval timer 사용 (일반적인 Unix 환경) */
#define USE_GETTOD 1   /* gettimeofday (any Unix box) */
                      /* gettimeofday 사용 (일반적인 Unix 환경) */

#endif /* __CONFIG_H */
