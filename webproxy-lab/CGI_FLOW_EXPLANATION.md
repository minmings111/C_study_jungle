# Tiny CGI Flow Explanation

이 문서는 `webproxy-lab/tiny/tiny.c` 와 `webproxy-lab/tiny/cgi-bin/adder.c` 가
어떻게 함께 동작하는지 설명합니다.

특히 아래 질문에 답하는 것이 목표입니다.

1. 왜 `tiny.c`에도 `main()`이 있고 `adder.c`에도 `main()`이 있는가
2. 두 파일은 함수 호출 관계인가, 아니면 별도 프로그램 관계인가
3. 브라우저 요청이 들어왔을 때 어떤 순서로 코드가 실행되는가
4. `fork`, `execve`, `QUERY_STRING`, `Dup2`가 각각 무슨 역할을 하는가

## 1. 먼저 결론부터

`tiny.c`와 `adder.c`는 같은 프로그램 안의 함수 관계가 아닙니다.

둘은 서로 다른 실행 파일입니다.

- `tiny.c`
  - 웹 서버 프로그램 `tiny`를 만듭니다.
- `adder.c`
  - CGI 프로그램 `adder`를 만듭니다.

즉:

- `tiny`는 서버입니다.
- `adder`는 서버가 필요할 때 실행하는 별도 프로그램입니다.

그래서 두 파일 모두 `main()`이 있어도 전혀 이상하지 않습니다.

각 프로그램은 자기 자신만의 시작점이 필요하기 때문입니다.

## 2. 왜 `main()`이 두 개 있어도 괜찮은가

C에서는 "실행 파일 하나"마다 `main()`이 하나 필요합니다.

여기서 중요한 점은:

- `tiny.c`는 `tiny`라는 실행 파일로 빌드됩니다.
- `adder.c`는 `adder`라는 실행 파일로 빌드됩니다.

즉 서로 다른 바이너리이므로 각각 자기 `main()`을 가져도 됩니다.

예를 들어 이 디렉터리에서는 실제로 이렇게 나뉩니다.

- `tiny/Makefile`
  - `tiny.c`를 빌드해서 `tiny` 실행 파일을 만듭니다.
- `tiny/cgi-bin/Makefile`
  - `adder.c`를 빌드해서 `adder` 실행 파일을 만듭니다.

그래서:

- `tiny` 실행 시 시작점은 `tiny.c`의 `main()`
- `adder` 실행 시 시작점은 `adder.c`의 `main()`

입니다.

## 3. 두 파일의 관계를 아주 짧게 말하면

두 파일의 관계는 아래와 같습니다.

- `tiny.c`
  - HTTP 요청을 받는 서버 프로그램
- `adder.c`
  - 특정 URI 요청이 들어왔을 때 Tiny가 실행하는 CGI 프로그램

즉:

- `tiny.c`가 `adder.c` 안의 함수를 직접 호출하는 것이 아닙니다.
- `tiny.c`가 운영체제에게 "`adder`라는 다른 프로그램을 실행해라"라고 요청하는 구조입니다.

이 차이가 아주 중요합니다.

## 4. 전체 흐름 한눈에 보기

브라우저에서 아래 URL을 요청한다고 가정하겠습니다.

```text
http://localhost:8000/cgi-bin/adder?1&2
```

이 요청이 들어오면 큰 흐름은 아래와 같습니다.

1. 브라우저가 Tiny 서버에 요청을 보냅니다.
2. `tiny`가 요청줄을 읽습니다.
3. URI에 `cgi-bin`이 있으므로 동적 콘텐츠 요청이라고 판단합니다.
4. `tiny`가 자식 프로세스를 만듭니다.
5. 그 자식 프로세스에서 `adder` 실행 파일을 실행합니다.
6. `adder`의 `main()`이 시작됩니다.
7. `adder`가 `QUERY_STRING`에서 `1&2`를 읽습니다.
8. `adder`가 계산 결과를 표준 출력으로 출력합니다.
9. 그 출력이 브라우저 응답으로 전달됩니다.

## 5. 코드 기준으로 실제 흐름 따라가기

### 5-1. 서버 시작

먼저 사용자가 Tiny 서버를 실행합니다.

```bash
./tiny 8000
```

그러면 [tiny.c](/home/leeminjeong/workspace/python_project/jungle/data_structures_docker/webproxy-lab/tiny/tiny.c:30) 의 `main()`이 시작됩니다.

이 `main()`의 역할:

- 포트를 열고
- 클라이언트 연결을 기다리고
- 연결이 오면 요청 하나를 처리하는 것

즉, 이 `main()`은 "웹 서버 프로그램의 시작점"입니다.

### 5-2. 요청 수락

브라우저가 `/cgi-bin/adder?1&2` 요청을 보내면 Tiny는 연결을 받아들입니다.

핵심 코드는 [tiny.c](/home/leeminjeong/workspace/python_project/jungle/data_structures_docker/webproxy-lab/tiny/tiny.c:48) 근처입니다.

```c
connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
doit(connfd);
```

여기서 의미는:

- `Accept(...)`
  - 브라우저와 연결된 소켓을 하나 만든다
- `doit(connfd)`
  - 그 연결에서 HTTP 요청 1개를 처리한다

### 5-3. 요청줄 읽기

[tiny.c](/home/leeminjeong/workspace/python_project/jungle/data_structures_docker/webproxy-lab/tiny/tiny.c:92) 의 `doit()`는 요청의 첫 줄을 읽습니다.

예를 들면 이런 줄입니다.

```text
GET /cgi-bin/adder?1&2 HTTP/1.0
```

그리고 이 줄을 분해해서:

- method = `GET`
- uri = `/cgi-bin/adder?1&2`
- version = `HTTP/1.0`

로 저장합니다.

### 5-4. 정적 요청인지 동적 요청인지 구분

그 다음 [tiny.c](/home/leeminjeong/workspace/python_project/jungle/data_structures_docker/webproxy-lab/tiny/tiny.c:111) 의 `parse_uri()`가 호출됩니다.

이 함수는 URI 안에 `cgi-bin`이 있는지 검사합니다.

- `cgi-bin`이 없으면 정적 콘텐츠
- `cgi-bin`이 있으면 동적 콘텐츠

지금은 URI가 `/cgi-bin/adder?1&2` 이므로 동적 콘텐츠입니다.

그래서:

- 실행 파일 경로: `./cgi-bin/adder`
- CGI 인자 문자열: `1&2`

로 나뉘게 됩니다.

## 6. `serve_dynamic()`에서 진짜 중요한 일

동적 요청이면 [tiny.c](/home/leeminjeong/workspace/python_project/jungle/data_structures_docker/webproxy-lab/tiny/tiny.c:389) 의 `serve_dynamic()`이 호출됩니다.

여기가 핵심입니다.

```c
if(Fork() == 0){
    setenv("QUERY_STRING", cgiargs, 1);
    Dup2(fd, STDOUT_FILENO);
    Execve(filename, emptylist, environ);
}
Wait(NULL);
```

이 네 줄이 CGI의 핵심입니다.

### 6-1. `Fork()`

`Fork()`는 현재 프로세스를 복제해서 자식 프로세스를 만듭니다.

왜 자식을 만들까?

이유는 Tiny 서버 본체와 CGI 프로그램 실행을 분리하기 위해서입니다.

부모 프로세스:

- 여전히 Tiny 서버 역할을 유지

자식 프로세스:

- 이제 CGI 프로그램을 실행할 준비를 함

즉, 웹 서버가 자기 몸을 완전히 `adder`로 바꾸는 것이 아니라,
"자식 하나를 만들어 그 자식에게 CGI를 맡긴다"라고 생각하면 됩니다.

### 6-2. `setenv("QUERY_STRING", cgiargs, 1)`

이 줄은 CGI 프로그램이 사용할 입력 값을 환경변수로 넣어 주는 작업입니다.

현재 예시에서는:

- `cgiargs = "1&2"`

이므로 결과적으로 자식 프로세스 환경에:

```text
QUERY_STRING=1&2
```

가 들어갑니다.

왜 이렇게 하냐면, 전통적인 CGI 프로그램은 URL 뒤의 쿼리 문자열을
`QUERY_STRING` 환경변수로 전달받기 때문입니다.

즉, Tiny는 `adder`에게 직접 함수 인자로 `1`, `2`를 넘기는 것이 아니라,
운영체제 환경변수 형태로 넘깁니다.

### 6-3. `Dup2(fd, STDOUT_FILENO)`

이 줄도 매우 중요합니다.

뜻은:

- 현재 클라이언트 소켓 `fd`를
- 표준 출력 `STDOUT_FILENO`로 연결한다

는 것입니다.

쉽게 말하면:

"이제부터 CGI 프로그램이 `printf`로 화면에 출력한다고 생각하는 모든 내용은
실제로는 브라우저에게 보내라"

는 뜻입니다.

그래서 `adder.c` 안에서 `printf(...)`를 하면,
그 출력이 터미널이 아니라 브라우저 응답으로 갑니다.

### 6-4. `Execve(filename, emptylist, environ)`

이 줄이 진짜로 `adder` 프로그램을 실행하는 부분입니다.

여기서 `filename`은 보통:

```text
./cgi-bin/adder
```

입니다.

`Execve`가 호출되면 자식 프로세스의 현재 실행 코드는 사라지고,
그 자리에 `adder` 실행 파일이 로드됩니다.

즉, 이 순간부터 자식 프로세스는 Tiny 코드가 아니라
`adder` 프로그램 자체가 됩니다.

그리고 그 프로그램의 시작점인 `main()`이 실행됩니다.

바로 이 때문에 `adder.c`에 `main()`이 필요한 것입니다.

## 7. 그래서 `adder.c`의 `main()`은 언제 실행되는가

정답은:

- 사용자가 `adder.c`를 직접 함수 호출할 때가 아니라
- Tiny가 `Execve("./cgi-bin/adder", ...)`를 했을 때

입니다.

즉, `adder.c`의 `main()`은 Tiny 안에서 직접 호출되는 것이 아니라
"새 프로그램이 시작되면서 자동으로 실행되는 진입점"입니다.

이걸 아주 짧게 쓰면:

- `tiny.c`의 `main()`은 서버 시작점
- `adder.c`의 `main()`은 CGI 프로그램 시작점

입니다.

## 8. `adder.c`는 그 안에서 무슨 일을 하나

[adder.c](/home/leeminjeong/workspace/python_project/jungle/data_structures_docker/webproxy-lab/tiny/cgi-bin/adder.c:8) 의 `main()`은 대략 이런 순서로 동작합니다.

1. `getenv("QUERY_STRING")`으로 쿼리 문자열을 읽는다
2. `&`를 기준으로 두 숫자를 분리한다
3. 문자열 `"1"`, `"2"`를 정수 `1`, `2`로 바꾼다
4. 계산 결과를 HTML 본문 문자열로 만든다
5. `printf`로 HTTP 헤더와 본문을 출력한다

예를 들어:

```c
buf = getenv("QUERY_STRING");
```

를 호출하면 Tiny가 미리 넣어 둔:

```text
QUERY_STRING=1&2
```

에서 `"1&2"`를 읽게 됩니다.

그 다음:

```c
p = strchr(buf, '&');
*p = '\0';
```

를 통해 원래 `"1&2"`였던 문자열을 메모리에서:

- `"1"`
- `"2"`

처럼 쪼개서 사용합니다.

그리고 마지막에:

```c
printf("Content-type: text/html\r\n\r\n");
printf("%s", content);
```

를 실행하면 그 내용이 브라우저로 전달됩니다.

왜냐하면 Tiny가 이미 `Dup2(fd, STDOUT_FILENO)`를 해 두었기 때문입니다.

## 9. `printf`가 왜 브라우저로 가는가

초보자 입장에서는 이 부분이 가장 신기할 수 있습니다.

보통 `printf`는 터미널에 출력된다고 생각하기 쉽습니다.

그런데 CGI에서는 다릅니다.

Tiny가 `Dup2(fd, STDOUT_FILENO)`를 호출한 뒤에는
표준 출력이 더 이상 터미널이 아니라 "클라이언트와 연결된 소켓"이 됩니다.

그래서 `adder`가 표준 출력으로 내보낸 내용은:

- 터미널이 아니라
- 브라우저 응답으로 전송됩니다

즉:

- `adder`는 그냥 `printf`만 하고
- Tiny가 그 출력을 브라우저로 가게 길을 바꿔 둔 것

입니다.

## 10. 함수 호출 관계가 아닌 이유

많이 헷갈리는 지점이라 다시 분명히 적으면:

아래는 아닙니다.

```c
// 이런 구조가 아님
doit() -> serve_dynamic() -> adder_main()
```

실제 구조는 이쪽에 가깝습니다.

```text
tiny 프로그램 실행 중
  -> fork
  -> 자식 프로세스 생성
  -> execve("./cgi-bin/adder")
  -> adder 프로그램 시작
  -> adder의 main() 실행
```

즉:

- `adder.c`는 `tiny.c`에 링크된 함수 파일이 아니라
- 별도 실행 파일로 존재하는 프로그램입니다

## 11. 비유로 이해하기

비유하면 이렇게 볼 수 있습니다.

- `tiny`
  - 식당 점원
- `adder`
  - 주문 들어오면 따로 부르는 요리사

흐름은:

1. 손님이 주문함
2. 점원이 주문 내용을 확인함
3. "이건 요리사에게 맡겨야 하는 주문이네"라고 판단함
4. 요리사를 불러서 주문 내용을 전달함
5. 요리사가 결과물을 만들어 냄
6. 그 결과물이 손님에게 전달됨

여기서 중요한 점:

- 점원이 직접 요리를 하는 것이 아닙니다
- 요리사가 별도 역할을 맡고 있습니다

Tiny와 CGI 프로그램 관계도 이와 비슷합니다.

## 12. 이 구조의 장점

이 구조의 장점은 역할이 잘 분리된다는 것입니다.

- Tiny 서버
  - HTTP 요청 수락
  - 요청 종류 판별
  - 정적/동적 분기
  - CGI 실행 환경 준비

- CGI 프로그램
  - 자기 로직 실행
  - 필요한 계산 수행
  - 응답 본문 출력

즉, 서버는 "웹 요청 처리"에 집중하고,
CGI 프로그램은 "실제 작업"에 집중합니다.

## 13. 마지막 정리

이 디렉터리에서 `main()`이 두 개 있는 이유는
"서로 다른 실행 파일이 두 개 있기 때문"입니다.

정리하면:

1. `tiny.c`의 `main()`
   - 웹 서버 프로그램 `tiny`의 시작점
2. `adder.c`의 `main()`
   - CGI 프로그램 `adder`의 시작점
3. Tiny는 동적 요청이 오면 `fork` 후 `execve`로 `adder`를 실행한다
4. Tiny는 `QUERY_STRING` 환경변수로 입력을 넘긴다
5. Tiny는 `Dup2`로 `adder`의 표준 출력을 브라우저 응답으로 연결한다
6. 그래서 `adder`의 `printf` 결과가 브라우저로 간다

가장 짧게 한 문장으로 말하면:

`adder.c`는 Tiny 안의 함수 파일이 아니라, Tiny가 필요할 때 실행하는 별도 CGI 프로그램이다.`

