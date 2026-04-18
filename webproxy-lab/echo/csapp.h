/*
 * csapp.h - CS:APP 교재에서 제공하는 도우미 함수들의 선언 모음
 *
 * 이 파일은 "실제 구현"이 아니라 "인터페이스 설명서" 역할을 한다.
 * 즉, 어떤 함수가 존재하는지, 어떤 자료형과 상수가 있는지를
 * 여러 C 파일이 공통으로 알 수 있게 해 준다.
 *
 * echo 예제에서 특히 중요한 것은:
 * - rio_t 구조체
 * - Rio_readinitb, Rio_readlineb, Rio_writen
 * - Open_clientfd, Open_listenfd
 *
 * 실제 함수 본문은 csapp.c에 들어 있다.
 */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

/* $begin csapp.h */
#ifndef __CSAPP_H__
#define __CSAPP_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* 파일 생성 시 기본 권한 계산에 쓰는 상수들이다. */
/* $begin createmasks */
#define DEF_MODE   S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH
#define DEF_UMASK  S_IWGRP|S_IWOTH
/* $end createmasks */

/* struct sockaddr를 코드에서 짧게 쓰기 위한 별칭이다. */
/* $begin sockaddrdef */
typedef struct sockaddr SA;
/* $end sockaddrdef */

/*
 * Rio 패키지가 읽기 상태를 계속 기억하기 위해 사용하는 구조체이다.
 *
 * 왜 필요한가:
 * - 네트워크나 파일에서 한 줄씩 읽을 때는
 *   내부 버퍼 상태를 기억해야 한다.
 * - 어느 fd에서 읽는지, 버퍼에 몇 바이트 남았는지,
 *   다음 읽기 위치가 어디인지 등을 함께 저장해야 한다.
 */
/* $begin rio_t */
#define RIO_BUFSIZE 8192
typedef struct {
    int rio_fd;                /* 이 rio가 연결된 파일 디스크립터 */
    int rio_cnt;               /* 내부 버퍼에 아직 읽지 않은 바이트 수 */
    char *rio_bufptr;          /* 다음에 읽을 내부 버퍼 위치 */
    char rio_buf[RIO_BUFSIZE]; /* 실제 내부 버퍼 공간 */
} rio_t;
/* $end rio_t */

/* 외부에서 제공되는 전역 변수들 */
extern int h_errno;    /* DNS 관련 오류 코드를 담는 전역 변수 */
extern char **environ; /* 현재 프로세스의 환경 변수 목록 */

/* 자주 쓰는 버퍼 크기, 줄 길이, listen 대기열 크기이다. */
#define	MAXLINE	 8192  /* 텍스트 한 줄 최대 길이 */
#define MAXBUF   8192  /* 일반 버퍼 최대 크기 */
#define LISTENQ  1024  /* listen() backlog 기본값 */

/* 공통 에러 처리 함수들 */
void unix_error(char *msg);
void posix_error(int code, char *msg);
void dns_error(char *msg);
void gai_error(int code, char *msg);
void app_error(char *msg);

/* 프로세스 제어 wrapper 함수들 */
pid_t Fork(void);
void Execve(const char *filename, char *const argv[], char *const envp[]);
pid_t Wait(int *status);
pid_t Waitpid(pid_t pid, int *iptr, int options);
void Kill(pid_t pid, int signum);
unsigned int Sleep(unsigned int secs);
void Pause(void);
unsigned int Alarm(unsigned int seconds);
void Setpgid(pid_t pid, pid_t pgid);
pid_t Getpgrp();

/* 시그널 관련 wrapper 함수들 */
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);
void Sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
void Sigemptyset(sigset_t *set);
void Sigfillset(sigset_t *set);
void Sigaddset(sigset_t *set, int signum);
void Sigdelset(sigset_t *set, int signum);
int Sigismember(const sigset_t *set, int signum);
int Sigsuspend(const sigset_t *set);

/* 시그널 핸들러 안에서도 비교적 안전하게 쓰기 위한 Sio 함수들 */
ssize_t sio_puts(char s[]);
ssize_t sio_putl(long v);
void sio_error(char s[]);

/* Sio wrapper 함수들 */
ssize_t Sio_puts(char s[]);
ssize_t Sio_putl(long v);
void Sio_error(char s[]);

/* Unix 입출력 wrapper 함수들 */
int Open(const char *pathname, int flags, mode_t mode);
ssize_t Read(int fd, void *buf, size_t count);
ssize_t Write(int fd, const void *buf, size_t count);
off_t Lseek(int fildes, off_t offset, int whence);
void Close(int fd);
int Select(int  n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, 
	   struct timeval *timeout);
int Dup2(int fd1, int fd2);
void Stat(const char *filename, struct stat *buf);
void Fstat(int fd, struct stat *buf) ;

/* 디렉터리 관련 wrapper 함수들 */
DIR *Opendir(const char *name);
struct dirent *Readdir(DIR *dirp);
int Closedir(DIR *dirp);

/* 메모리 매핑 wrapper 함수들 */
void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
void Munmap(void *start, size_t length);

/* 표준 입출력 wrapper 함수들 */
void Fclose(FILE *fp);
FILE *Fdopen(int fd, const char *type);
char *Fgets(char *ptr, int n, FILE *stream);
FILE *Fopen(const char *filename, const char *mode);
void Fputs(const char *ptr, FILE *stream);
size_t Fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
void Fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

/* 동적 메모리 할당 wrapper 함수들 */
void *Malloc(size_t size);
void *Realloc(void *ptr, size_t size);
void *Calloc(size_t nmemb, size_t size);
void Free(void *ptr);

/* socket, bind, listen, accept, connect를 감싼 wrapper 함수들이다. */
int Socket(int domain, int type, int protocol);
void Setsockopt(int s, int level, int optname, const void *optval, int optlen);
void Bind(int sockfd, struct sockaddr *my_addr, int addrlen);
void Listen(int s, int backlog);
int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);
void Connect(int sockfd, struct sockaddr *serv_addr, int addrlen);

/* IPv4/IPv6를 직접 크게 의식하지 않고 쓰게 도와주는 주소 관련 함수들이다. */
void Getaddrinfo(const char *node, const char *service, 
                 const struct addrinfo *hints, struct addrinfo **res);
void Getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host, 
                 size_t hostlen, char *serv, size_t servlen, int flags);
void Freeaddrinfo(struct addrinfo *res);
void Inet_ntop(int af, const void *src, char *dst, socklen_t size);
void Inet_pton(int af, const char *src, void *dst); 

/* 오래된 DNS 관련 wrapper 함수들 */
struct hostent *Gethostbyname(const char *name);
struct hostent *Gethostbyaddr(const char *addr, int len, int type);

/* Pthreads 스레드 제어 wrapper 함수들 */
void Pthread_create(pthread_t *tidp, pthread_attr_t *attrp, 
		    void * (*routine)(void *), void *argp);
void Pthread_join(pthread_t tid, void **thread_return);
void Pthread_cancel(pthread_t tid);
void Pthread_detach(pthread_t tid);
void Pthread_exit(void *retval);
pthread_t Pthread_self(void);
void Pthread_once(pthread_once_t *once_control, void (*init_function)());

/* POSIX 세마포어 wrapper 함수들 */
void Sem_init(sem_t *sem, int pshared, unsigned int value);
void P(sem_t *sem);
void V(sem_t *sem);

/*
 * 소문자 rio_*는 Robust I/O의 실제 구현 함수들이다.
 * 에러를 반환값으로 돌려주므로 호출한 쪽이 직접 확인해야 한다.
 */
ssize_t rio_readn(int fd, void *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);
void rio_readinitb(rio_t *rp, int fd); 
ssize_t	rio_readnb(rio_t *rp, void *usrbuf, size_t n);
ssize_t	rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);

/*
 * 대문자 Rio_*는 소문자 rio_*를 감싼 wrapper 함수들이다.
 * echo 예제에서는 주로 이 대문자 버전을 사용한다.
 */
ssize_t Rio_readn(int fd, void *usrbuf, size_t n);
void Rio_writen(int fd, void *usrbuf, size_t n);
void Rio_readinitb(rio_t *rp, int fd); 
ssize_t Rio_readnb(rio_t *rp, void *usrbuf, size_t n);
ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);

/*
 * 소문자 open_* 함수들은 클라이언트 연결과 서버 리스닝 소켓 준비를 돕는 helper이다.
 * 실패 시 오류 코드를 반환한다.
 */
int open_clientfd(char *hostname, char *port);
int open_listenfd(char *port);

/*
 * 대문자 Open_*는 open_* helper의 wrapper 버전이다.
 * echo 예제에서 직접 호출하는 것은 보통 이 대문자 버전이다.
 */
int Open_clientfd(char *hostname, char *port);
int Open_listenfd(char *port);


#endif /* __CSAPP_H__ 헤더 중복 포함 방지 끝 */
/* $end csapp.h */
