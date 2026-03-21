# SAGEDash Runtime / 배포 정책

## 개요

SAGEDash는 MFC 네이티브 실행 파일(`.exe`)과 WebView2 웹 자산(`web/`)을 함께 배포한다.
이 문서는 빌드 산출물 구조, 웹 자산 배포 방식, WebView2 런타임 정책을 정의한다.

---

## 배포 산출물 구조

```
SAGEDash/               ← 배포 루트 (exe와 같은 디렉터리)
├── SAGEDash.exe
├── WebView2Loader.dll
├── web/
│   ├── style.css       ← 공통 스타일 시트
│   ├── bridge.js       ← 공통 브릿지 모듈
│   ├── dashboard.html
│   ├── report.html
│   └── analysis.html
└── (기타 MFC 런타임 DLL)
```

### 핵심 규칙
- `web/` 폴더는 항상 `SAGEDash.exe`와 **같은 디렉터리**에 위치해야 한다.
- `SAGEDash.exe`가 웹 파일 경로를 `GetModuleFileName()` 기준 상대 경로로 계산하기 때문이다.
- `web/` 내부 파일은 서로 상대 경로(`src="bridge.js"`, `href="style.css"`)로 참조하며, 절대 경로를 쓰지 않는다.

---

## 웹 자산 빌드 연동

### 현재 방식 (수동 복사)
- Visual Studio 빌드 후 `$(OutDir)web\` 디렉터리에 `web/` 파일을 수동으로 복사한다.
- Debug/Release 모두 동일한 방법을 적용한다.

### 권장 방식 (Post-Build Event)
`SAGEDash.vcxproj` 의 Post-Build Event에 아래 커맨드를 추가하면 빌드 시 자동 복사된다.

```
xcopy /E /Y /I "$(SolutionDir)web" "$(OutDir)web"
```

> **설정 위치**: 프로젝트 속성 → 빌드 이벤트 → 빌드 후 이벤트 → 명령줄

---

## WebView2 런타임 정책

### Evergreen (권장)
- 사용자 PC에 WebView2 Evergreen 런타임이 설치되어 있으면 자동으로 사용한다.
- 최신 Chromium 기반 엔진을 자동 업데이트로 유지한다.
- Windows 11 이상에서는 기본 포함되어 있다.

### Fixed Version (옵션)
- 특정 버전의 WebView2 런타임을 `external/webview2-runtime/` 에 번들링하여 배포할 수 있다.
- 환경 격리가 필요한 사내 배포 환경에서 사용한다.
- `WebView2EnvironmentOptions`에서 `BrowserExecutableFolder`를 지정한다.

### 설치 확인
앱 시작 시 `GetAvailableCoreWebView2BrowserVersionString()`으로 런타임 존재를 확인하고,
미설치 시 부트스트래퍼 URL 안내 또는 Offline 설치 파일 실행을 제공한다.

---

## 웹 자산 파일 변경 정책

| 파일 | 역할 | 변경 시 주의사항 |
|------|------|----------------|
| `bridge.js` | C++ ↔ Web 브릿지 라우터 | `SageBridge.on` / `SageBridge.send` API 시그니처 변경 시 C++ 상수도 함께 업데이트 |
| `style.css` | 공통 디자인 시스템 | CSS 변수명 변경 시 모든 HTML 파일 영향 확인 |
| `dashboard.html` | 파일 요약 대시보드 | `data:summary` 페이로드 구조와 동기화 |
| `report.html` | 검증 결과 리포트 | `data:validation-report` 페이로드 구조와 동기화 |
| `analysis.html` | 컬럼 분석 | `data:column-analysis` 페이로드 구조와 동기화 |

---

## 브릿지 메시지 규약 참조

브릿지 메시지 타입 상수는 `src/ui/webview/WebBridgeMessage.h`의 `BRIDGE_TYPE_*` 상수로 관리된다.
웹 파일의 메시지 타입 문자열은 이 상수와 항상 동기화되어야 한다.

자세한 규약은 `docs/decisions/BRIDGE_PROTOCOL.md`를 참조한다.

---

## Phase별 배포 상태

| Phase | 배포 항목 |
|-------|----------|
| Phase 1~3 | exe + MFC 런타임만 |
| Phase 4 | exe + WebView2Loader.dll + web/ 추가 |
| Phase 5~6 | 동일 구조 유지, web/ 파일 증가 |
| v0.5 | 현재 단계 — develop 브랜치 기준 |
| v1.0 | 인스톨러 패키지 (NSIS 또는 WiX) 검토 예정 |
