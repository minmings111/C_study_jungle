/*
 * ftimer.c - Estimate the time (in seconds) used by a function f 
 * 
 * Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
 * May not be used, modified, or copied without permission.
 *
 * Function timers that estimate the running time (in seconds) of a function f.
 *    ftimer_itimer: version that uses the interval timer
 *    ftimer_gettod: version that uses gettimeofday
 */
/*
 * ftimer.c - 함수 f가 사용한 시간을 초 단위로 추정한다.
 *
 * Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
 * 허가 없이 사용, 수정, 복사할 수 없습니다.
 *
 * 함수 실행 시간 측정기:
 *    ftimer_itimer: interval timer를 사용하는 버전
 *    ftimer_gettod: gettimeofday를 사용하는 버전
 */
#include <stdio.h>
#include <sys/time.h>
#include "ftimer.h"

/* function prototypes */
/* 함수 원형 선언 */
static void init_etime(void);
static double get_etime(void);

/* 
 * ftimer_itimer - Use the interval timer to estimate the running time
 * of f(argp). Return the average of n runs.  
 */
/*
 * ftimer_itimer - interval timer를 사용해 f(argp)의 실행 시간을 추정한다.
 *                 n번 실행한 평균값을 반환한다.
 */
double ftimer_itimer(ftimer_test_funct f, void *argp, int n)
{
    double start, tmeas;
    int i;

    init_etime();
    start = get_etime();
    for (i = 0; i < n; i++) 
	f(argp);
    tmeas = get_etime() - start;
    return tmeas / n;
}

/* 
 * ftimer_gettod - Use gettimeofday to estimate the running time of
 * f(argp). Return the average of n runs.  
 */
/*
 * ftimer_gettod - gettimeofday를 사용해 f(argp)의 실행 시간을 추정한다.
 *                 n번 실행한 평균값을 반환한다.
 */
double ftimer_gettod(ftimer_test_funct f, void *argp, int n)
{
    int i;
    struct timeval stv, etv;
    double diff;

    gettimeofday(&stv, NULL);
    for (i = 0; i < n; i++) 
	f(argp);
    gettimeofday(&etv,NULL);
    diff = 1E3*(etv.tv_sec - stv.tv_sec) + 1E-3*(etv.tv_usec-stv.tv_usec);
    diff /= n;
    return (1E-3*diff);
}


/*
 * Routines for manipulating the Unix interval timer
 */
/*
 * Unix interval timer를 다루는 루틴들
 */

/* The initial value of the interval timer */
/* interval timer의 초기값 */
#define MAX_ETIME 86400   

/* static variables that hold the initial value of the interval timer */
/* interval timer의 초기값을 보관하는 정적 변수들 */
static struct itimerval first_u; /* user time */
                                 /* 사용자 시간 */
static struct itimerval first_r; /* real time */
                                 /* 실제 시간 */
static struct itimerval first_p; /* prof time*/
                                 /* 프로파일링 시간 */

/* init the timer */
/* 타이머를 초기화한다 */
static void init_etime(void)
{
    first_u.it_interval.tv_sec = 0;
    first_u.it_interval.tv_usec = 0;
    first_u.it_value.tv_sec = MAX_ETIME;
    first_u.it_value.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, &first_u, NULL);

    first_r.it_interval.tv_sec = 0;
    first_r.it_interval.tv_usec = 0;
    first_r.it_value.tv_sec = MAX_ETIME;
    first_r.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &first_r, NULL);
   
    first_p.it_interval.tv_sec = 0;
    first_p.it_interval.tv_usec = 0;
    first_p.it_value.tv_sec = MAX_ETIME;
    first_p.it_value.tv_usec = 0;
    setitimer(ITIMER_PROF, &first_p, NULL);
}

/* return elapsed real seconds since call to init_etime */
/* init_etime 호출 이후 경과한 실제 시간을 초 단위로 반환한다 */
static double get_etime(void) {
    struct itimerval v_curr;
    struct itimerval r_curr;
    struct itimerval p_curr;

    getitimer(ITIMER_VIRTUAL, &v_curr);
    getitimer(ITIMER_REAL,&r_curr);
    getitimer(ITIMER_PROF,&p_curr);

    return (double) ((first_p.it_value.tv_sec - r_curr.it_value.tv_sec) +
		     (first_p.it_value.tv_usec - r_curr.it_value.tv_usec)*1e-6);
}



