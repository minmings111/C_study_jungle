/* Routines for using cycle counter */
/* 사이클 카운터를 사용하기 위한 함수들 */

/* Start the counter */
/* 카운터를 시작한다 */
void start_counter();

/* Get # cycles since counter started */
/* 카운터 시작 이후 지난 사이클 수를 얻는다 */
double get_counter();

/* Measure overhead for counter */
/* 카운터 자체의 오버헤드를 측정한다 */
double ovhd();

/* Determine clock rate of processor (using a default sleeptime) */
/* 기본 sleeptime을 사용해 프로세서의 클럭 속도를 구한다 */
double mhz(int verbose);

/* Determine clock rate of processor, having more control over accuracy */
/* 정확도를 더 세밀하게 제어하며 프로세서의 클럭 속도를 구한다 */
double mhz_full(int verbose, int sleeptime);

/** Special counters that compensate for timer interrupt overhead */
/** 타이머 인터럽트 오버헤드를 보정하는 특수 카운터 */

void start_comp_counter();

double get_comp_counter();
