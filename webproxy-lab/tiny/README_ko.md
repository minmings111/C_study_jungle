Tiny Web server
Dave O'Hallaron
Carnegie Mellon University

이 디렉터리는 Tiny 서버의 홈 디렉터리입니다. Tiny는 약 200줄 정도의 간단한 웹 서버로,
Carnegie Mellon University의 "15-213: Intro to Computer Systems" 수업에서 사용됩니다.
Tiny는 GET 메서드를 사용해 현재 디렉터리(`./`)의 정적 콘텐츠
(텍스트, HTML, GIF, JPG 파일)를 제공하고, `./cgi-bin`에 있는 CGI 프로그램을 실행해
동적 콘텐츠를 제공합니다. 기본 페이지는 `index.html`이 아니라 `home.html`인데,
이는 브라우저에서 디렉터리 내용을 확인하기 쉽도록 하기 위해서입니다.

Tiny는 보안적으로 안전하지도 않고 기능이 완전하지도 않지만,
학생들이 실제 웹 서버가 어떻게 동작하는지 감을 잡는 데 도움이 됩니다.
교육용으로만 사용하세요.

이 코드는 Linux 2.2.20 커널 환경에서 gcc 2.95.3을 사용해
문제 없이 컴파일 및 실행되도록 작성되었습니다.

Tiny 설치 방법:
   비어 있는 디렉터리에서 `tar xvf tiny.tar`를 실행하세요.

Tiny 실행 방법:
   서버 머신에서 `tiny <port>`를 실행하세요.
   예: `tiny 8000`
   그런 다음 브라우저에서 Tiny에 접속합니다.
   정적 콘텐츠: `http://<host>:8000`
   동적 콘텐츠: `http://<host>:8000/cgi-bin/adder?1&2`

파일 설명:
  `tiny.tar`            이 디렉터리 전체가 들어 있는 아카이브
  `tiny.c`              Tiny 서버 소스 코드
  `Makefile`            `tiny.c`용 메이크파일
  `home.html`           테스트용 HTML 페이지
  `godzilla.gif`        `home.html`에 포함된 이미지
  `README`              이 문서
  `cgi-bin/adder.c`     두 수를 더하는 CGI 프로그램
  `cgi-bin/Makefile`    `adder.c`용 메이크파일
