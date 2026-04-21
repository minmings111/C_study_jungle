# Tiny Troubleshooting Log

이 문서는 `webproxy-lab/tiny` 서버를 점검하면서 발생한 문제와,
그 문제를 어떤 방식으로 분석하고 해결했는지를 상세히 기록한 문서입니다.

특히 아래 3가지를 중심으로 정리합니다.

1. 이전 코드가 왜 문제였는가
2. `snprintf` 방식으로 무엇을 바꾸었는가
3. 바꾼 뒤 실제로 어떤 증상이 해결되었는가

이 문서는 "정답 코드"만 적는 문서가 아니라,
실제로 어떤 시행착오를 겪었고 왜 그 결론에 도달했는지를 남기는 기록입니다.

## 1. 처음 보였던 증상

브라우저와 `curl`로 Tiny를 테스트했을 때, 아래와 같은 이상 현상이 보였습니다.

### 1-1. 브라우저에서 이상한 로그가 반복됨

서버 터미널에 아래처럼 비슷한 문자열이 계속 보였습니다.

```text
st/html
xt/html
/html
```

또는 같은 요청이 반복해서 들어오는 것처럼 보였습니다.

예:

```text
GET / HTTP/1.1
GET / HTTP/1.1
GET / HTTP/1.1
```

### 1-2. 정적 파일 요청이 브라우저 기준으로 정상처럼 보이지 않음

예를 들어 `/sample.txt` 같은 정적 파일을 요청했을 때,
서버 로그는 요청을 받은 것처럼 보였지만 브라우저/클라이언트 쪽에서는
응답 처리가 이상하게 보였습니다.

`curl -i`로 보면 다음과 같은 에러가 나왔습니다.

```text
curl: (1) Received HTTP/0.9 when not allowed
```

이 메시지는 보통 "응답이 HTTP/1.0 또는 HTTP/1.1 형식의 정상 헤더처럼 보이지 않는다"는 뜻으로 해석할 수 있습니다.

### 1-3. CGI `adder`는 실행되지만 결과가 이상함

`/cgi-bin/adder?1&2` 요청을 보냈을 때,
CGI는 실행되는 것처럼 보였지만 본문이 예상과 달랐습니다.

예상:

```text
QUERY_STRING=1&2
...
The answer is: 1 + 2 = 3
```

실제:

```text
Thanks for visiting!
```

또는 앞부분이 사라진 채 마지막 문장만 남는 식의 결과가 나왔습니다.

## 2. 원인 분석: 어디가 문제였는가

문제는 `Accept`나 `Rio_readlineb` 같은 "연결 수락" 단계가 아니었습니다.

실제로 확인해보면:

- 서버는 정상적으로 열림
- `Accept`는 정상
- 요청줄 읽기 정상
- URI 파싱도 정상

즉 요청을 **받는 단계**는 대체로 문제 없었습니다.

문제의 핵심은 **응답 문자열 조립**이었습니다.

## 3. 가장 큰 문제: 같은 버퍼를 입력과 출력에 동시에 사용함

이전 코드에서 가장 위험했던 패턴은 아래와 같았습니다.

```c
sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
```

또는:

```c
sprintf(body, "%s<p>%s : %s\r\n", body, longmsg, cause);
```

또는:

```c
sprintf(content, "%sThe answer is: %d + %d = %d\r\n<p>", content, n1, n2, n1+n2);
```

이 코드는 공통적으로 다음 문제를 갖고 있습니다.

- 출력 대상도 `buf` 또는 `body` 또는 `content`
- 입력 문자열로 읽는 것도 같은 `buf` 또는 `body` 또는 `content`

즉:

```c
sprintf(destination, "%s...", destination);
```

형태입니다.

이 패턴은 안전하지 않습니다.

왜냐하면:

1. `sprintf`가 결과를 같은 버퍼에 덮어쓰기 시작하는 순간
2. 그 버퍼의 기존 내용도 동시에 읽어야 하므로
3. 읽기와 쓰기가 서로 영향을 주게 됩니다

즉, "기존 문자열을 뒤에 이어 붙인다"는 의도로 작성했지만,
실제로는 메모리 겹침 때문에 문자열 일부가 사라지거나 마지막 조각만 남는 현상이 생길 수 있습니다.

## 4. 정적 응답에서 정확히 무엇이 깨졌는가

정적 응답을 만드는 `serve_static()`의 이전 코드는 이랬습니다.

```c
sprintf(buf, "HTTP/1.0 200 OK\r\n");
sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
sprintf(buf, "%sConnection: close\r\n", buf);
sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
```

의도는:

1. 상태 줄 넣기
2. `Server` 헤더 추가
3. `Connection` 헤더 추가
4. `Content-length` 추가
5. `Content-type` 추가

였습니다.

하지만 실제로는 각 줄마다 `buf`를 다시 읽고 다시 쓰기 때문에,
앞에서 만든 문자열이 안정적으로 유지된다는 보장이 없었습니다.

그래서 raw 응답을 보면 실제로는 이런 식이었습니다.

```text
Content-type: text/html

<html>...
```

즉 원래 있어야 할:

```text
HTTP/1.0 200 OK
Server: Tiny Web Server
Connection: close
Content-length: ...
```

가 사라지고, 마지막 헤더 조각만 남는 현상이 나타났습니다.

이 상태에서는 브라우저가 응답을 "정상적인 HTTP 응답"으로 해석하지 못할 수 있습니다.

그 결과:

- 문서 요청 재시도
- 로그 반복
- `curl -i` 실패

같은 현상이 발생했습니다.

## 5. CGI 응답 본문에서 무엇이 깨졌는가

`adder.c`의 이전 본문 조립은 이랬습니다.

```c
sprintf(content, "QUERY_STRING=%s", buf);
sprintf(content, "Welcom to add.com: ");
sprintf(content, "%sThe Internet addition partal.\r\n<p>", content);
sprintf(content, "%sThe answer is: %d + %d = %d\r\n<p>", content, n1, n2, n1+n2);
sprintf(content, "%sThanks for visiting!\r\n", content);
```

여기에는 두 종류의 문제가 섞여 있었습니다.

### 5-1. 바로 덮어쓰는 문제

첫 줄:

```c
sprintf(content, "QUERY_STRING=%s", buf);
```

로 `QUERY_STRING`을 넣어도,

바로 다음 줄:

```c
sprintf(content, "Welcom to add.com: ");
```

가 전체를 다시 써버립니다.

즉 첫 줄은 그 순간 바로 사라집니다.

### 5-2. 자기 자신 기반 문자열 덧붙이기 문제

그 뒤 줄들도:

```c
sprintf(content, "%s...", content);
```

형태였기 때문에 같은 메모리 겹침 문제가 있었습니다.

그래서 실제로는:

- 앞부분이 사라짐
- 마지막 문장만 남음
- 조립 순서가 의도대로 유지되지 않음

같은 현상이 생길 수 있었습니다.

## 6. 왜 `snprintf`로 바꿨는가

이번 수정에서는 핵심 문자열 조립을 `snprintf`로 바꿨습니다.

이 선택의 이유는 두 가지입니다.

1. 같은 버퍼를 입력과 출력에 동시에 쓰는 패턴을 없애기 위해
2. 버퍼 크기를 넘기지 않도록 길이 제한까지 함께 걸기 위해

즉 `snprintf`는:

- 메모리 겹침을 피할 수 있는 작성 방식으로 바꾸기 좋고
- 최대 크기까지 명시할 수 있어서 더 안전합니다

## 7. 정적 응답은 어떻게 바뀌었는가

`serve_static()`는 아래처럼 바뀌었습니다.

```c
snprintf(buf, sizeof(buf),
         "HTTP/1.0 200 OK\r\n"
         "Server: Tiny Web Server\r\n"
         "Connection: close\r\n"
         "Content-length: %d\r\n"
         "Content-type: %s\r\n\r\n",
         filesize, filetype);
```

이 방식의 핵심은:

- `buf`를 한 번에 완성한다는 점
- 중간에 같은 `buf`를 입력 문자열로 다시 읽지 않는다는 점

즉 이전처럼:

```c
sprintf(buf, "%s...", buf);
```

패턴이 없어졌습니다.

이제는 상태줄과 주요 헤더가 한 번에 안정적으로 만들어집니다.

## 8. CGI 본문은 어떻게 바뀌었는가

`adder.c`는 아래처럼 바뀌었습니다.

```c
snprintf(content, sizeof(content),
         "QUERY_STRING=%s\r\n"
         "Welcom to add.com: "
         "The Internet addition partal.\r\n<p>"
         "The answer is: %d + %d = %d\r\n<p>"
         "Thanks for visiting!\r\n",
         buf, n1, n2, n1+n2);
```

이 방식의 장점:

- 문자열 전체를 한 번에 조립
- 중간에 기존 `content`를 다시 읽지 않음
- 첫 줄이 다음 줄에 의해 덮어써지지 않음

즉 "순서대로 조합된 최종 본문"을 바로 만드는 구조로 바뀌었습니다.

## 9. 에러 본문도 같은 방식으로 고쳤는가

네. `clienterror()`의 `body` 조립도 같은 이유로 바뀌었습니다.

이전:

```c
sprintf(body, "%s...", body);
```

이후:

```c
snprintf(body, sizeof(body), ...);
```

즉 정적 응답, CGI 본문, 에러 본문 모두
"같은 버퍼를 입력/출력에 동시에 쓰는 패턴"을 제거하는 방향으로 정리했습니다.

## 10. 변경 후 실제 테스트 결과

변경 후 `tiny`를 실행하고 아래를 확인했습니다.

### 10-1. 정적 `/` 요청

```bash
curl -i http://127.0.0.1:4500/
```

결과:

```text
HTTP/1.0 200 OK
Server: Tiny Web Server
Connection: close
Content-length: 120
Content-type: text/html
```

즉 이전처럼 `Content-type`만 남는 것이 아니라,
정상적인 상태줄과 헤더가 모두 보였습니다.

### 10-2. 정적 `/sample.txt` 요청

```bash
curl -i http://127.0.0.1:4500/sample.txt
```

결과:

```text
HTTP/1.0 200 OK
Server: Tiny Web Server
Connection: close
Content-length: 130
Content-type: text/plain
```

정적 텍스트 파일도 정상 헤더로 내려왔습니다.

### 10-3. CGI `/cgi-bin/adder?1&2` 요청

```bash
curl -i "http://127.0.0.1:4500/cgi-bin/adder?1&2"
```

결과:

```text
HTTP/1.0 200 OK
Server: Tiny Web Server
Connection: close
Content-length: 120
Content-type: text/html

QUERY_STRING=1
Welcom to add.com: The Internet addition partal.
<p>The answer is: 1 + 2 = 3
<p>Thanks for visiting!
```

즉:

- CGI 헤더 정상
- 덧셈 결과 정상
- 본문 마지막 문장만 남는 문제 해결

## 11. 그런데 왜 `QUERY_STRING=1`인가

이건 현재 코드의 다른 특성 때문입니다.

`adder.c`는 먼저:

```c
p = strchr(buf, '&');
*p = '\0';
```

를 실행해서 `"1&2"`를 메모리에서 `"1"`과 `"2"`로 쪼갭니다.

그래서 이후 `buf`는 더 이상 `"1&2"` 전체가 아니라 `"1"`만 가리키게 됩니다.

즉 지금 출력되는:

```text
QUERY_STRING=1
```

은 `snprintf` 문제와는 별개입니다.

원래 쿼리 문자열 전체를 보여주고 싶다면,
`*p = '\0'` 하기 전에 원본을 따로 복사해 둬야 합니다.

## 12. 시행착오 요약

이번 문제 해결 과정은 대략 이렇게 진행되었습니다.

1. 브라우저에서 로그가 이상하게 반복됨
2. 처음에는 `Accept` 이후 흐름 전체가 의심됨
3. 하지만 서버는 열리고 요청도 잘 읽는다는 사실 확인
4. `curl -i`로 보니 정적 응답이 정상 HTTP 형식이 아님
5. raw 응답을 보니 `Content-type`만 남아 있음
6. `serve_static()`의 `sprintf(buf, "%s...", buf)` 패턴이 핵심 원인으로 좁혀짐
7. `adder.c`도 같은 패턴이라 CGI 본문도 같이 깨진다는 점 확인
8. `snprintf`로 "한 번에 완성"하는 방식으로 변경
9. 정적 헤더와 CGI 본문이 정상으로 돌아옴

즉 핵심 교훈은:

- 문제는 네트워크 연결 수락 단계가 아니라
- 응답 문자열을 버퍼에 조립하는 방식에 있었다

입니다.

## 13. 왜 이런 예제 코드가 문제를 일으킬 수 있는가

Tiny 예제는 교육용입니다.

즉:

- 네트워크 서버의 큰 흐름 설명
- 정적/동적 콘텐츠 개념 설명
- `fork`, `execve`, CGI 동작 설명

에 초점이 있습니다.

그래서 최신 컴파일러, 최신 브라우저, 엄격한 문자열 안전성 기준까지
항상 완벽히 맞춰져 있다고 보기는 어렵습니다.

이번 문제도 그런 종류의 예입니다.

즉:

- 코드 흐름 설명용으로는 이해하기 좋지만
- 문자열 조립 방식은 실제로는 더 안전하게 고쳐 써야 했습니다

## 14. 최종 정리

이전 코드의 핵심 문제는
"같은 버퍼를 입력 문자열과 출력 버퍼로 동시에 사용하는 `sprintf` 패턴"이었습니다.

그 결과:

- 정적 응답 헤더가 깨졌고
- 브라우저가 정상 HTTP 응답으로 해석하지 못했고
- CGI 본문도 앞부분이 사라졌습니다

이번 수정에서는:

- `serve_static()`의 헤더 조립
- `clienterror()`의 에러 본문 조립
- `adder.c`의 CGI 본문 조립

을 `snprintf` 기반으로 바꿔,
"문자열 전체를 한 번에 안전하게 만드는 방식"으로 정리했습니다.

그 결과:

- 정적 파일 응답 정상
- CGI 응답 정상
- 브라우저와 `curl`이 이해할 수 있는 HTTP 응답 복구

가 확인되었습니다.
