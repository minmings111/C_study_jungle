/*
 * fcyc.h - prototypes for the routines in fcyc.c that estimate the
 *     time in CPU cycles used by a test function f
 * 
 * Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
 * May not be used, modified, or copied without permission.
 *
 */
/*
 * fcyc.h - 테스트 함수 f가 사용한 CPU 사이클 수를 추정하는
 *          fcyc.c의 루틴 원형들
 *
 * Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
 * 허가 없이 사용, 수정, 복사할 수 없습니다.
 *
 */

/* The test function takes a generic pointer as input */
/* 테스트 함수는 일반 포인터 하나를 입력으로 받는다 */
typedef void (*test_funct)(void *);

/* Compute number of cycles used by test function f */
/* 테스트 함수 f가 사용한 사이클 수를 계산한다 */
double fcyc(test_funct f, void* argp);

/*********************************************************
 * Set the various parameters used by measurement routines 
 *********************************************************/
/*********************************************************
 * 측정 루틴에서 사용하는 여러 파라미터를 설정한다
 *********************************************************/

/* 
 * set_fcyc_clear_cache - When set, will run code to clear cache 
 *     before each measurement. 
 *     Default = 0
 */
/*
 * set_fcyc_clear_cache - 설정되면 각 측정 전에 캐시를 비우는
 *                        코드를 실행한다.
 *                        기본값 = 0
 */
void set_fcyc_clear_cache(int clear);

/* 
 * set_fcyc_cache_size - Set size of cache to use when clearing cache 
 *     Default = 1<<19 (512KB)
 */
/*
 * set_fcyc_cache_size - 캐시를 비울 때 사용할 캐시 크기를 설정한다.
 *                       기본값 = 1<<19 (512KB)
 */
void set_fcyc_cache_size(int bytes);

/* 
 * set_fcyc_cache_block - Set size of cache block 
 *     Default = 32
 */
/*
 * set_fcyc_cache_block - 캐시 블록 크기를 설정한다.
 *                        기본값 = 32
 */
void set_fcyc_cache_block(int bytes);

/* 
 * set_fcyc_compensate- When set, will attempt to compensate for 
 *     timer interrupt overhead 
 *     Default = 0
 */
/*
 * set_fcyc_compensate - 설정되면 타이머 인터럽트 오버헤드를
 *                       보정하려고 시도한다.
 *                       기본값 = 0
 */
void set_fcyc_compensate(int compensate_arg);

/* 
 * set_fcyc_k - Value of K in K-best measurement scheme
 *     Default = 3
 */
/*
 * set_fcyc_k - K-best 측정 방식에서 K 값을 설정한다.
 *              기본값 = 3
 */
void set_fcyc_k(int k);

/* 
 * set_fcyc_maxsamples - Maximum number of samples attempting to find 
 *     K-best within some tolerance.
 *     When exceeded, just return best sample found.
 *     Default = 20
 */
/*
 * set_fcyc_maxsamples - 허용 오차 안에서 K-best를 찾기 위해 시도할
 *                       최대 샘플 수를 설정한다.
 *                       이를 넘기면 찾은 최적 샘플을 그대로 반환한다.
 *                       기본값 = 20
 */
void set_fcyc_maxsamples(int maxsamples_arg);

/* 
 * set_fcyc_epsilon - Tolerance required for K-best
 *     Default = 0.01
 */
/*
 * set_fcyc_epsilon - K-best에 필요한 허용 오차를 설정한다.
 *                    기본값 = 0.01
 */
void set_fcyc_epsilon(double epsilon_arg);



