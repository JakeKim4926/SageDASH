# WebView2 환경 설정

**날짜**: 2026-03-17
**관련 Phase**: Phase 4 — WebView2 파일럿
**브랜치**: feature/webview2-setup

---

## 결정 사항

SAGEDash에 WebView2를 도입하기 위해 **NuGet 패키지** 방식으로 빌드 환경을 구성한다.

---

## 패키지 정보

| 항목 | 값 |
|------|-----|
| 패키지 ID | `Microsoft.Web.WebView2` |
| 버전 | `1.0.2849.39` |
| 방식 | `packages.config` + vcxproj 수동 import |
| 설치 경로 | `$(SolutionDir)packages\Microsoft.Web.WebView2.1.0.2849.39\` |

---

## 빌드 설정 변경 내용

### `SAGEDash/packages.config` (신규)
NuGet 패키지 참조 선언.

### `SAGEDash/SAGEDash.vcxproj` 변경
- `ExtensionSettings` 에 WebView2 `.props` import 추가
  - 헤더 경로(`$(WebView2IncludePath)`) 자동 주입
- `ExtensionTargets` 에 WebView2 `.targets` import + 빌드 오류 검증 Target 추가
  - `WebView2Loader.dll` 자동 복사 (출력 폴더)

---

## WebView2Loader.dll 배포 경로

`Microsoft.Web.WebView2.targets` 가 빌드 후 자동으로 출력 디렉터리에 복사한다.

- **Debug x64**: `$(SolutionDir)Debug_x64\WebView2Loader.dll`
- **Release x64**: `$(SolutionDir)Release_x64\WebView2Loader.dll`

별도 post-build 이벤트 불필요.

---

## 빌드 전 필수 작업

NuGet 패키지 복원이 완료되지 않으면 빌드 시 명확한 오류 메시지가 출력된다.

```
Visual Studio → 솔루션 우클릭 → NuGet 패키지 복원
```

또는 명령줄:
```
cd D:\projects\sagedash
nuget restore SAGEDash\packages.config -PackagesDirectory packages
```

---

## 이후 단계 참조

이 설정은 Phase 4-2 (`WebViewHost` 래퍼 구현)의 전제 조건이다.
패키지 복원 후 `WebView2.h` 헤더 포함 가능 여부를 먼저 확인한다.

```cpp
#include <WebView2.h>  // packages\...\build\native\include\
```
