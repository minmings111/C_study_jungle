/* 
 * Function timers 
 */
/*
 * 함수 실행 시간 측정기
 */
typedef void (*ftimer_test_funct)(void *); 

/* Estimate the running time of f(argp) using the Unix interval timer.
   Return the average of n runs */
/* Unix interval timer를 이용해 f(argp)의 실행 시간을 추정한다.
   n번 실행한 평균값을 반환한다 */
double ftimer_itimer(ftimer_test_funct f, void *argp, int n);


/* Estimate the running time of f(argp) using gettimeofday 
   Return the average of n runs */
/* gettimeofday를 이용해 f(argp)의 실행 시간을 추정한다.
   n번 실행한 평균값을 반환한다 */
double ftimer_gettod(ftimer_test_funct f, void *argp, int n);
