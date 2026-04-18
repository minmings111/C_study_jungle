/*
 * csapp.c - CS:APP 교재용 도우미 함수 구현 파일
 *
 * 변경 이력:
 * 2016-10
 * - sio_ltoa가 음수를 제대로 처리하지 못하던 버그 수정
 *
 * 2016-02
 * - open_clientfd, open_listenfd가 좀 더 부드럽게 실패하도록 개선
 *
 * 2014-08
 * - open_clientfd, open_listenfd를 재진입 가능하고
 *   프로토콜 독립적인 버전으로 개선
 * - 프로토콜 독립적인 inet_ntop, inet_pton 함수 추가
 * - inet_ntoa, inet_aton은 더 이상 권장되지 않음
 *
 * 2014-07
 * - 재진입 가능한 sio(signal-safe I/O) 루틴 추가
 *
 * 2013-04
 * - rio_readlineb의 경계 조건 버그 수정
 * - rio_readnb의 중복 EINTR 검사 제거
 */
/* $begin csapp.c */
#include "csapp.h"

/**************************
 * 에러 처리 함수들
 **************************/
/* $begin errorfuns */
/* $begin unixerror */
void unix_error(char *msg) /* Unix 계열 시스템 호출 오류 */
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}
/* $end unixerror */

void posix_error(int code, char *msg) /* POSIX 스타일 오류 */
{
    fprintf(stderr, "%s: %s\n", msg, strerror(code));
    exit(0);
}

void gai_error(int code, char *msg) /* getaddrinfo 계열 오류 */
{
    fprintf(stderr, "%s: %s\n", msg, gai_strerror(code));
    exit(0);
}

void app_error(char *msg) /* 일반 애플리케이션 오류 */
{
    fprintf(stderr, "%s\n", msg);
    exit(0);
}
/* $end errorfuns */

void dns_error(char *msg) /* 오래된 gethostbyname 계열 오류 */
{
    fprintf(stderr, "%s\n", msg);
    exit(0);
}


/*********************************************
 * Unix 프로세스 제어 wrapper 함수들
 ********************************************/

/* $begin forkwrapper */
pid_t Fork(void) 
{
    pid_t pid;

    if ((pid = fork()) < 0)
	unix_error("Fork error");
    return pid;
}
/* $end forkwrapper */

void Execve(const char *filename, char *const argv[], char *const envp[]) 
{
    if (execve(filename, argv, envp) < 0)
	unix_error("Execve error");
}

/* $begin wait */
pid_t Wait(int *status) 
{
    pid_t pid;

    if ((pid  = wait(status)) < 0)
	unix_error("Wait error");
    return pid;
}
/* $end wait */

pid_t Waitpid(pid_t pid, int *iptr, int options) 
{
    pid_t retpid;

    if ((retpid  = waitpid(pid, iptr, options)) < 0) 
	unix_error("Waitpid error");
    return(retpid);
}

/* $begin kill */
void Kill(pid_t pid, int signum) 
{
    int rc;

    if ((rc = kill(pid, signum)) < 0)
	unix_error("Kill error");
}
/* $end kill */

void Pause() 
{
    (void)pause();
    return;
}

unsigned int Sleep(unsigned int secs) 
{
    unsigned int rc;

    if ((rc = sleep(secs)) < 0)
	unix_error("Sleep error");
    return rc;
}

unsigned int Alarm(unsigned int seconds) {
    return alarm(seconds);
}
 
void Setpgid(pid_t pid, pid_t pgid) {
    int rc;

    if ((rc = setpgid(pid, pgid)) < 0)
	unix_error("Setpgid error");
    return;
}

pid_t Getpgrp(void) {
    return getpgrp();
}

/************************************
 * Unix 시그널 wrapper 함수들
 ***********************************/

/* $begin sigaction */
handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* 현재 처리 중인 시그널과 같은 종류를 잠시 막음 */
    action.sa_flags = SA_RESTART; /* 가능하면 중단된 시스템 호출을 다시 시작 */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
}
/* $end sigaction */

void Sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
    if (sigprocmask(how, set, oldset) < 0)
	unix_error("Sigprocmask error");
    return;
}

void Sigemptyset(sigset_t *set)
{
    if (sigemptyset(set) < 0)
	unix_error("Sigemptyset error");
    return;
}

void Sigfillset(sigset_t *set)
{ 
    if (sigfillset(set) < 0)
	unix_error("Sigfillset error");
    return;
}

void Sigaddset(sigset_t *set, int signum)
{
    if (sigaddset(set, signum) < 0)
	unix_error("Sigaddset error");
    return;
}

void Sigdelset(sigset_t *set, int signum)
{
    if (sigdelset(set, signum) < 0)
	unix_error("Sigdelset error");
    return;
}

int Sigismember(const sigset_t *set, int signum)
{
    int rc;
    if ((rc = sigismember(set, signum)) < 0)
	unix_error("Sigismember error");
    return rc;
}

int Sigsuspend(const sigset_t *set)
{
    int rc = sigsuspend(set); /* 정상 동작해도 반환값은 항상 -1 */
    if (errno != EINTR)
        unix_error("Sigsuspend error");
    return rc;
}

/*************************************************************
 * Sio (Signal-safe I/O) 패키지
 *
 * 시그널 핸들러 안에서도 비교적 안전하게 쓸 수 있는
 * 단순한 재진입 가능 출력 함수들이다.
 *************************************************************/

/* 내부에서만 사용하는 Sio 함수들 */

/* $begin sioprivate */
/* sio_reverse - 문자열 뒤집기 (K&R 예제 기반) */
static void sio_reverse(char s[])
{
    int c, i, j;

    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* sio_ltoa - long 값을 b진수 문자열로 바꾸기 (K&R 예제 기반) */
static void sio_ltoa(long v, char s[], int b) 
{
    int c, i = 0;
    int neg = v < 0;

    if (neg)
	v = -v;

    do {  
        s[i++] = ((c = (v % b)) < 10)  ?  c + '0' : c - 10 + 'a';
    } while ((v /= b) > 0);

    if (neg)
	s[i++] = '-';

    s[i] = '\0';
    sio_reverse(s);
}

/* sio_strlen - 문자열 길이 구하기 (K&R 예제 기반) */
static size_t sio_strlen(char s[])
{
    int i = 0;

    while (s[i] != '\0')
        ++i;
    return i;
}
/* $end sioprivate */

/* 외부에서 호출하는 Sio 함수들 */
/* $begin siopublic */

ssize_t sio_puts(char s[]) /* 문자열 출력 */
{
    return write(STDOUT_FILENO, s, sio_strlen(s)); //line:csapp:siostrlen
}

ssize_t sio_putl(long v) /* long 정수 출력 */
{
    char s[128];
    
    sio_ltoa(v, s, 10); /* K&R의 itoa 아이디어를 바탕으로 변환 */  //line:csapp:sioltoa
    return sio_puts(s);
}

void sio_error(char s[]) /* 에러 메시지를 출력하고 종료 */
{
    sio_puts(s);
    _exit(1);                                      //line:csapp:sioexit
}
/* $end siopublic */

/*******************************
 * SIO 루틴용 wrapper 함수들
 ******************************/
ssize_t Sio_putl(long v)
{
    ssize_t n;
  
    if ((n = sio_putl(v)) < 0)
	sio_error("Sio_putl error");
    return n;
}

ssize_t Sio_puts(char s[])
{
    ssize_t n;
  
    if ((n = sio_puts(s)) < 0)
	sio_error("Sio_puts error");
    return n;
}

void Sio_error(char s[])
{
    sio_error(s);
}

/********************************
 * Unix 입출력 루틴 wrapper 함수들
 ********************************/

int Open(const char *pathname, int flags, mode_t mode) 
{
    int rc;

    if ((rc = open(pathname, flags, mode))  < 0)
	unix_error("Open error");
    return rc;
}

ssize_t Read(int fd, void *buf, size_t count) 
{
    ssize_t rc;

    if ((rc = read(fd, buf, count)) < 0) 
	unix_error("Read error");
    return rc;
}

ssize_t Write(int fd, const void *buf, size_t count) 
{
    ssize_t rc;

    if ((rc = write(fd, buf, count)) < 0)
	unix_error("Write error");
    return rc;
}

off_t Lseek(int fildes, off_t offset, int whence) 
{
    off_t rc;

    if ((rc = lseek(fildes, offset, whence)) < 0)
	unix_error("Lseek error");
    return rc;
}

void Close(int fd) 
{
    int rc;

    if ((rc = close(fd)) < 0)
	unix_error("Close error");
}

int Select(int  n, fd_set *readfds, fd_set *writefds,
	   fd_set *exceptfds, struct timeval *timeout) 
{
    int rc;

    if ((rc = select(n, readfds, writefds, exceptfds, timeout)) < 0)
	unix_error("Select error");
    return rc;
}

int Dup2(int fd1, int fd2) 
{
    int rc;

    if ((rc = dup2(fd1, fd2)) < 0)
	unix_error("Dup2 error");
    return rc;
}

void Stat(const char *filename, struct stat *buf) 
{
    if (stat(filename, buf) < 0)
	unix_error("Stat error");
}

void Fstat(int fd, struct stat *buf) 
{
    if (fstat(fd, buf) < 0)
	unix_error("Fstat error");
}

/*********************************
 * 디렉터리 함수 wrapper
 *********************************/

DIR *Opendir(const char *name) 
{
    DIR *dirp = opendir(name); 

    if (!dirp)
        unix_error("opendir error");
    return dirp;
}

struct dirent *Readdir(DIR *dirp)
{
    struct dirent *dep;
    
    errno = 0;
    dep = readdir(dirp);
    if ((dep == NULL) && (errno != 0))
        unix_error("readdir error");
    return dep;
}

int Closedir(DIR *dirp) 
{
    int rc;

    if ((rc = closedir(dirp)) < 0)
        unix_error("closedir error");
    return rc;
}

/***************************************
 * 메모리 매핑 함수 wrapper
 ***************************************/
void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset) 
{
    void *ptr;

    if ((ptr = mmap(addr, len, prot, flags, fd, offset)) == ((void *) -1))
	unix_error("mmap error");
    return(ptr);
}

void Munmap(void *start, size_t length) 
{
    if (munmap(start, length) < 0)
	unix_error("munmap error");
}

/***************************************************
 * 동적 메모리 할당 함수 wrapper
 ***************************************************/

void *Malloc(size_t size) 
{
    void *p;

    if ((p  = malloc(size)) == NULL)
	unix_error("Malloc error");
    return p;
}

void *Realloc(void *ptr, size_t size) 
{
    void *p;

    if ((p  = realloc(ptr, size)) == NULL)
	unix_error("Realloc error");
    return p;
}

void *Calloc(size_t nmemb, size_t size) 
{
    void *p;

    if ((p = calloc(nmemb, size)) == NULL)
	unix_error("Calloc error");
    return p;
}

void Free(void *ptr) 
{
    free(ptr);
}

/******************************************
 * 표준 입출력 함수 wrapper
 ******************************************/
void Fclose(FILE *fp) 
{
    if (fclose(fp) != 0)
	unix_error("Fclose error");
}

FILE *Fdopen(int fd, const char *type) 
{
    FILE *fp;

    if ((fp = fdopen(fd, type)) == NULL)
	unix_error("Fdopen error");

    return fp;
}

char *Fgets(char *ptr, int n, FILE *stream) 
{
    char *rptr;

    if (((rptr = fgets(ptr, n, stream)) == NULL) && ferror(stream))
	app_error("Fgets error");

    return rptr;
}

FILE *Fopen(const char *filename, const char *mode) 
{
    FILE *fp;

    if ((fp = fopen(filename, mode)) == NULL)
	unix_error("Fopen error");

    return fp;
}

void Fputs(const char *ptr, FILE *stream) 
{
    if (fputs(ptr, stream) == EOF)
	unix_error("Fputs error");
}

size_t Fread(void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
    size_t n;

    if (((n = fread(ptr, size, nmemb, stream)) < nmemb) && ferror(stream)) 
	unix_error("Fread error");
    return n;
}

void Fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
    if (fwrite(ptr, size, nmemb, stream) < nmemb)
	unix_error("Fwrite error");
}


/****************************
 * 소켓 인터페이스 wrapper
 ****************************/

int Socket(int domain, int type, int protocol) 
{
    int rc;

    if ((rc = socket(domain, type, protocol)) < 0)
	unix_error("Socket error");
    return rc;
}

void Setsockopt(int s, int level, int optname, const void *optval, int optlen) 
{
    int rc;

    if ((rc = setsockopt(s, level, optname, optval, optlen)) < 0)
	unix_error("Setsockopt error");
}

void Bind(int sockfd, struct sockaddr *my_addr, int addrlen) 
{
    int rc;

    if ((rc = bind(sockfd, my_addr, addrlen)) < 0)
	unix_error("Bind error");
}

void Listen(int s, int backlog) 
{
    int rc;

    if ((rc = listen(s,  backlog)) < 0)
	unix_error("Listen error");
}

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen) 
{
    int rc;

    if ((rc = accept(s, addr, addrlen)) < 0)
	unix_error("Accept error");
    return rc;
}

void Connect(int sockfd, struct sockaddr *serv_addr, int addrlen) 
{
    int rc;

    if ((rc = connect(sockfd, serv_addr, addrlen)) < 0)
	unix_error("Connect error");
}

/*******************************
 * 프로토콜 독립적인 wrapper
 *******************************/
/* $begin getaddrinfo */
void Getaddrinfo(const char *node, const char *service, 
                 const struct addrinfo *hints, struct addrinfo **res)
{
    int rc;

    if ((rc = getaddrinfo(node, service, hints, res)) != 0) 
        gai_error(rc, "Getaddrinfo error");
}
/* $end getaddrinfo */

void Getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host, 
                 size_t hostlen, char *serv, size_t servlen, int flags)
{
    int rc;

    if ((rc = getnameinfo(sa, salen, host, hostlen, serv, 
                          servlen, flags)) != 0) 
        gai_error(rc, "Getnameinfo error");
}

void Freeaddrinfo(struct addrinfo *res)
{
    freeaddrinfo(res);
}

void Inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    if (!inet_ntop(af, src, dst, size))
        unix_error("Inet_ntop error");
}

void Inet_pton(int af, const char *src, void *dst) 
{
    int rc;

    rc = inet_pton(af, src, dst);
    if (rc == 0)
	app_error("inet_pton error: invalid dotted-decimal address");
    else if (rc < 0)
        unix_error("Inet_pton error");
}

/*******************************************
 * DNS 인터페이스 wrapper
 *
 * 주의:
 * 이 함수들은 스레드 안전하지 않아서 더 이상 권장되지 않는다.
 * 가능하면 getaddrinfo와 getnameinfo를 사용해야 한다.
 ***********************************/

/* $begin gethostbyname */
struct hostent *Gethostbyname(const char *name) 
{
    struct hostent *p;

    if ((p = gethostbyname(name)) == NULL)
	dns_error("Gethostbyname error");
    return p;
}
/* $end gethostbyname */

struct hostent *Gethostbyaddr(const char *addr, int len, int type) 
{
    struct hostent *p;

    if ((p = gethostbyaddr(addr, len, type)) == NULL)
	dns_error("Gethostbyaddr error");
    return p;
}

/************************************************
 * Pthreads 스레드 제어 wrapper 함수들
 ************************************************/

void Pthread_create(pthread_t *tidp, pthread_attr_t *attrp, 
		    void * (*routine)(void *), void *argp) 
{
    int rc;

    if ((rc = pthread_create(tidp, attrp, routine, argp)) != 0)
	posix_error(rc, "Pthread_create error");
}

void Pthread_cancel(pthread_t tid) {
    int rc;

    if ((rc = pthread_cancel(tid)) != 0)
	posix_error(rc, "Pthread_cancel error");
}

void Pthread_join(pthread_t tid, void **thread_return) {
    int rc;

    if ((rc = pthread_join(tid, thread_return)) != 0)
	posix_error(rc, "Pthread_join error");
}

/* $begin detach */
void Pthread_detach(pthread_t tid) {
    int rc;

    if ((rc = pthread_detach(tid)) != 0)
	posix_error(rc, "Pthread_detach error");
}
/* $end detach */

void Pthread_exit(void *retval) {
    pthread_exit(retval);
}

pthread_t Pthread_self(void) {
    return pthread_self();
}
 
void Pthread_once(pthread_once_t *once_control, void (*init_function)()) {
    pthread_once(once_control, init_function);
}

/*******************************
 * POSIX 세마포어 wrapper 함수들
 *******************************/

void Sem_init(sem_t *sem, int pshared, unsigned int value) 
{
    if (sem_init(sem, pshared, value) < 0)
	unix_error("Sem_init error");
}

void P(sem_t *sem) 
{
    if (sem_wait(sem) < 0)
	unix_error("P error");
}

void V(sem_t *sem) 
{
    if (sem_post(sem) < 0)
	unix_error("V error");
}

/****************************************
 * The Rio package - Robust I/O functions
 *
 * 이 섹션은 "조금 더 안정적으로 읽고 쓰기" 위한 함수들이다.
 *
 * echo 예제에서 중요한 흐름:
 * 1. Rio_readinitb로 rio_t를 fd와 연결한다.
 * 2. Rio_readlineb로 한 줄을 읽는다.
 * 3. Rio_writen으로 데이터를 다시 보낸다.
 ****************************************/

/*
 * rio_readn - 버퍼를 따로 쓰지 않고 n바이트를 끝까지 읽으려는 함수
 *
 * read()는 한 번 호출했다고 항상 원하는 길이만큼 읽어 주지 않는다.
 * 그래서 "아직 덜 읽었으면 계속 읽는다"는 반복 로직이 필요하다.
 */
/* $begin rio_readn */
ssize_t rio_readn(int fd, void *usrbuf, size_t n) 
{
    size_t nleft = n;     /* 아직 더 읽어야 하는 바이트 수 */
    ssize_t nread;        /* 이번 read()가 실제로 읽은 바이트 수 */
    char *bufp = usrbuf;  /* 사용자 버퍼 안에서 현재 써야 할 위치 */

    while (nleft > 0) {
	if ((nread = read(fd, bufp, nleft)) < 0) {
	    if (errno == EINTR) /* 시그널 핸들러 때문에 중간에 끊긴 경우 */
		nread = 0;      /* 읽기를 실패로 보지 말고 다시 시도 */
	    else
		return -1;      /* read()가 errno를 설정한 실제 오류 */
	} 
	else if (nread == 0)
	    break;              /* 파일 끝 또는 연결 종료 */
	nleft -= nread;
	bufp += nread;
    }
    return (n - nleft);         /* 0 이상인 실제 읽은 바이트 수 반환 */
}
/* $end rio_readn */

/*
 * rio_writen - n바이트를 끝까지 쓰려고 반복하는 함수
 *
 * write()도 한 번에 모든 데이터를 다 쓰지 못할 수 있으므로,
 * 남은 양이 없어질 때까지 반복 호출한다.
 */
/* $begin rio_writen */
ssize_t rio_writen(int fd, void *usrbuf, size_t n) 
{
    size_t nleft = n;     /* 아직 더 써야 하는 바이트 수 */
    ssize_t nwritten;     /* 이번 write()가 실제로 쓴 바이트 수 */
    char *bufp = usrbuf;  /* 사용자 버퍼 안에서 현재 읽을 위치 */

    while (nleft > 0) {
	if ((nwritten = write(fd, bufp, nleft)) <= 0) {
	    if (errno == EINTR)  /* 시그널 핸들러 때문에 중간에 끊긴 경우 */
		nwritten = 0;    /* 쓰기를 실패로 보지 말고 다시 시도 */
	    else
		return -1;       /* write()가 errno를 설정한 실제 오류 */
	}
	nleft -= nwritten;
	bufp += nwritten;
    }
    return n;
}
/* $end rio_writen */


/*
 * rio_read - 내부 버퍼를 사용하는 핵심 읽기 함수
 *
 * 이 함수는 외부에서 직접 자주 부르기보다는
 * rio_readnb와 rio_readlineb가 내부적으로 사용한다.
 *
 * 동작:
 * - 내부 버퍼가 비어 있으면 read()로 새로 채운다.
 * - 그 버퍼에서 필요한 만큼만 사용자 버퍼로 복사한다.
 */
/* $begin rio_read */
static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
    int cnt;  /* 이번에 사용자 버퍼로 실제 복사할 바이트 수 */

    while (rp->rio_cnt <= 0) {  /* 내부 버퍼가 비어 있으면 다시 채움 */
	rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, 
			   sizeof(rp->rio_buf));
	if (rp->rio_cnt < 0) {
	    if (errno != EINTR) /* 시그널 때문에 끊긴 경우가 아니면 진짜 오류 */
		return -1;
	}
	else if (rp->rio_cnt == 0)  /* 파일 끝 또는 연결 종료 */
	    return 0;
	else 
	    rp->rio_bufptr = rp->rio_buf; /* 버퍼 포인터를 처음으로 되돌림 */
    }

    /* 내부 버퍼에 남은 양과 요청량 중 더 작은 만큼만 사용자 버퍼로 복사 */
    cnt = n;          
    if (rp->rio_cnt < n)   
	cnt = rp->rio_cnt;
    memcpy(usrbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}
/* $end rio_read */

/*
 * rio_readinitb - rio_t 구조체를 특정 파일 디스크립터와 연결한다.
 *
 * echo 예제에서:
 * - 서버는 connfd와 연결한다.
 * - 클라이언트는 clientfd와 연결한다.
 */
/* $begin rio_readinitb */
void rio_readinitb(rio_t *rp, int fd) 
{
    rp->rio_fd = fd;          /* 어떤 fd에서 읽을지 저장 */
    rp->rio_cnt = 0;          /* 내부 버퍼에 남은 데이터가 없다고 초기화 */
    rp->rio_bufptr = rp->rio_buf; /* 버퍼 포인터를 시작 위치로 맞춘다 */
}
/* $end rio_readinitb */

/*
 * rio_readnb - 내부 버퍼를 이용해 n바이트를 읽는다.
 *
 * 줄 단위가 아니라 "정해진 크기" 기준 읽기이다.
 */
/* $begin rio_readnb */
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n) 
{
    size_t nleft = n;     /* 앞으로 더 읽어야 할 바이트 수 */
    ssize_t nread;        /* 이번 반복에서 읽은 바이트 수 */
    char *bufp = usrbuf;  /* 사용자 버퍼 안 현재 위치 */
    
    while (nleft > 0) {
	if ((nread = rio_read(rp, bufp, nleft)) < 0) 
            return -1;          /* read()가 errno를 설정한 실제 오류 */
	else if (nread == 0)
	    break;              /* 파일 끝 또는 연결 종료 */
	nleft -= nread;
	bufp += nread;
    }
    return (n - nleft);         /* 0 이상인 실제 읽은 바이트 수 반환 */
}
/* $end rio_readnb */

/*
 * rio_readlineb - 한 줄 텍스트를 읽는다.
 *
 * echo 예제에서 가장 중요한 함수 중 하나이다.
 * 개행 문자('\n')를 만날 때까지 읽어서 문자열 버퍼에 저장한다.
 */
/* $begin rio_readlineb */
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) 
{
    int n, rc;
    char c, *bufp = usrbuf;

    for (n = 1; n < maxlen; n++) { 
        if ((rc = rio_read(rp, &c, 1)) == 1) {
	    *bufp++ = c;
	    if (c == '\n') {
                n++;
     		break;
            }
	} else if (rc == 0) {
	    if (n == 1)
		return 0; /* 파일 끝, 아직 아무 데이터도 못 읽음 */
	    else
		break;    /* 파일 끝, 일부 데이터는 이미 읽음 */
	} else
	    return -1;	  /* 실제 오류 */
    }
    *bufp = 0;
    return n-1;
}
/* $end rio_readlineb */

/**********************************
 * Wrappers for robust I/O routines
 *
 * 소문자 rio_*는 오류 코드를 반환하는 저수준 함수이고,
 * 대문자 Rio_*는 실패 시 공통 에러 처리로 연결하는 wrapper이다.
 **********************************/
ssize_t Rio_readn(int fd, void *ptr, size_t nbytes) 
{
    ssize_t n;
  
    if ((n = rio_readn(fd, ptr, nbytes)) < 0)
	unix_error("Rio_readn error");
    return n;
}

/* Rio_writen - 실패 검사까지 포함한 wrapper */
void Rio_writen(int fd, void *usrbuf, size_t n) 
{
    if (rio_writen(fd, usrbuf, n) != n)
	unix_error("Rio_writen error");
}

/* Rio_readinitb - rio 초기화 wrapper */
void Rio_readinitb(rio_t *rp, int fd)
{
    rio_readinitb(rp, fd);
} 

/* Rio_readnb - n바이트 읽기 wrapper */
ssize_t Rio_readnb(rio_t *rp, void *usrbuf, size_t n) 
{
    ssize_t rc;

    if ((rc = rio_readnb(rp, usrbuf, n)) < 0)
	unix_error("Rio_readnb error");
    return rc;
}

/* Rio_readlineb - 한 줄 읽기 wrapper */
ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) 
{
    ssize_t rc;

    if ((rc = rio_readlineb(rp, usrbuf, maxlen)) < 0)
	unix_error("Rio_readlineb error");
    return rc;
} 

/******************************** 
 * Client/server helper functions
 *
 * echo 예제에서 "연결하기"와 "서버 열기"를 담당하는 핵심 helper이다.
 *
 * open_clientfd:
 * - 클라이언트가 서버에 연결할 때 사용
 *
 * open_listenfd:
 * - 서버가 특정 포트에서 연결을 기다릴 때 사용
 ********************************/
/*
 * open_clientfd - <hostname, port>로 연결되는 클라이언트 소켓을 연다.
 *
 * 큰 흐름:
 * 1. getaddrinfo로 가능한 주소 후보를 얻는다.
 * 2. 각 후보에 대해 socket()을 시도한다.
 * 3. connect()에 성공하는 후보가 나오면 그 소켓을 반환한다.
 *
 * echo 클라이언트에서는 이 반환값이 clientfd가 된다.
 */
/* $begin open_clientfd */
int open_clientfd(char *hostname, char *port) {
    int clientfd, rc;
    struct addrinfo hints, *listp, *p;

    /* getaddrinfo에 넘길 조건을 먼저 설정한다. */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;  /* TCP 스트림 소켓 사용 */
    hints.ai_flags = AI_NUMERICSERV;  /* port 인자가 숫자 문자열임을 알림 */
    hints.ai_flags |= AI_ADDRCONFIG;  /* 현재 시스템에서 쓸 수 있는 주소만 추천 */
    if ((rc = getaddrinfo(hostname, port, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo failed (%s:%s): %s\n", hostname, port, gai_strerror(rc));
        return -2;
    }
  
    /* 주소 후보를 하나씩 보면서 실제 연결에 성공하는 후보를 찾는다. */
    for (p = listp; p; p = p->ai_next) {
        /* 현재 후보에 맞는 소켓을 생성한다. */
        if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
            continue; /* 소켓 생성 실패, 다음 후보 시도 */

        /* 소켓 생성이 되면 실제 서버 연결을 시도한다. */
        if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1) 
            break; /* 연결 성공 */
        if (close(clientfd) < 0) { /* 실패한 소켓은 닫고 다음 후보를 본다. */  //line:netp:openclientfd:closefd
            fprintf(stderr, "open_clientfd: close failed: %s\n", strerror(errno));
            return -1;
        } 
    } 

    /* getaddrinfo가 만든 후보 목록 메모리 정리 */
    freeaddrinfo(listp);
    if (!p) /* 모든 후보 연결 실패 */
        return -1;
    else    /* 마지막 시도 연결 성공 */
        return clientfd;
}
/* $end open_clientfd */

/*  
 * open_listenfd - 특정 port에서 연결을 기다리는 리스닝 소켓을 연다.
 *
 * 큰 흐름:
 * 1. bind 가능한 주소 후보 목록을 얻는다.
 * 2. socket()으로 소켓을 만든다.
 * 3. bind()로 주소와 포트에 묶는다.
 * 4. listen()으로 연결 대기 상태로 만든다.
 *
 * echo 서버에서는 이 반환값이 listenfd가 된다.
 */
/* $begin open_listenfd */
int open_listenfd(char *port) 
{
    struct addrinfo hints, *listp, *p;
    int listenfd, rc, optval=1;

    /* 서버가 열 수 있는 주소 후보 목록을 얻기 위한 조건을 설정한다. */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;             /* TCP 연결 수락용 */
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* 현재 시스템의 사용 가능한 임의 주소에 바인드 가능 */
    hints.ai_flags |= AI_NUMERICSERV;            /* port 인자가 숫자 문자열임을 알림 */
    if ((rc = getaddrinfo(NULL, port, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo failed (port %s): %s\n", port, gai_strerror(rc));
        return -2;
    }

    /* bind에 성공하는 주소 후보를 찾을 때까지 반복한다. */
    for (p = listp; p; p = p->ai_next) {
        /* 현재 후보에 맞는 소켓을 만든다. */
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
            continue;  /* 소켓 생성 실패, 다음 후보 시도 */

        /* 같은 포트를 빠르게 다시 열 때 bind 오류를 줄이는 옵션이다. */
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,    //line:netp:csapp:setsockopt
                   (const void *)&optval , sizeof(int));

        /* 소켓을 실제 주소/포트에 묶는다. */
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break; /* bind 성공 */
        if (close(listenfd) < 0) { /* bind 실패, 현재 소켓 닫고 다음 후보 시도 */
            fprintf(stderr, "open_listenfd close failed: %s\n", strerror(errno));
            return -1;
        }
    }


    /* getaddrinfo가 만든 후보 목록 메모리 정리 */
    freeaddrinfo(listp);
    if (!p) /* 어느 주소 후보도 동작하지 않음 */
        return -1;

    /* 이제 이 소켓을 "연결 요청을 받아들이는 상태"로 바꾼다. */
    if (listen(listenfd, LISTENQ) < 0) {
        close(listenfd);
	return -1;
    }
    return listenfd;
}
/* $end open_listenfd */

/****************************************************
 * Wrappers for reentrant protocol-independent helpers
 *
 * 소문자 open_*은 오류 코드를 반환하는 helper이고,
 * 대문자 Open_*은 실패 시 바로 공통 에러 처리로 넘어가는 wrapper이다.
 * echo 예제에서는 보통 이 대문자 버전을 직접 호출한다.
 ****************************************************/
int Open_clientfd(char *hostname, char *port) 
{
    int rc;

    if ((rc = open_clientfd(hostname, port)) < 0) 
	unix_error("Open_clientfd error");
    return rc;
}

int Open_listenfd(char *port) 
{
    int rc;

    if ((rc = open_listenfd(port)) < 0)
	unix_error("Open_listenfd error");
    return rc;
}

/* $end csapp.c */
