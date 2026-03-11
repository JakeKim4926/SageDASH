# SAGEDash - Claude Code 가이드

## 프로젝트 개요

SAGEDash는 C++/MFC 기반의 **문서 중심 멀티 작업형 업무 자동화 플랫폼**이다.
엑셀/문서/정산/리포트 등 반복 업무를 자동화하며, 공통 기능은 무료로 제공하고 고객사별 요구사항은 템플릿/확장 모듈로 대응한다.

## 기술 스택

- **언어**: C++
- **프레임워크**: MFC (Microsoft Foundation Classes)
- **IDE/빌드**: Visual Studio 2026, x64 Debug/Release
- **프로젝트 구조**: MDI (Multiple Document Interface)

## 권장 목표 구조

현재는 VS 마법사가 생성한 초기 상태이며, 아래 구조로 발전시킨다.

```
SAGEDash/
├─ docs/                        # 문서
│  ├─ architecture/             # 아키텍처 문서
│  ├─ decisions/                # 설계 결정 기록
│  ├─ guides/                   # 가이드
│  └─ templates/                # 문서 템플릿
├─ external/                    # 서드파티 라이브러리
├─ resource/                    # 리소스 (icons, images, menus, strings)
├─ src/
│  ├─ app/                      # CSAGEDashApp, AppBootstrap
│  ├─ ui/
│  │  ├─ frame/                 # CMainFrame, CChildFrame
│  │  ├─ doc/                   # CSAGEDashDoc, document_types/
│  │  ├─ view/                  # CSAGEDashView, grid/, form/, report/
│  │  ├─ pane/                  # TemplateExplorerPane, OutputPane, PropertyPane
│  │  ├─ dialog/                # 다이얼로그
│  │  └─ controls/              # 커스텀 컨트롤
│  ├─ application/              # 서비스 계층 (services, coordinators, commands, jobs, usecases)
│  ├─ domain/                   # 도메인 계층 (models, rules, policies, value_objects, repositories)
│  ├─ infrastructure/           # 인프라 계층 (excel, config, logging, persistence, file_system, serialization)
│  ├─ common/                   # 공통 (base, error, threading, utils, types)
│  └─ tests/                    # 테스트 (domain, application, infrastructure)
├─ .gitignore
└─ SAGEDash.slnx
```

## 코딩 규칙 (필수)

### 금지 사항
- `auto` 키워드 사용 금지
- 스마트 포인터 사용 금지
- 함수 길이 200줄 초과 금지
- 매직 넘버 금지 → `Define.h`에 도메인별 주석 블록을 먼저 표시하고, 해당 주석 아래에 그 도메인의 상수를 선언
- Enum은 `EnumDefine.h`에 선언
- 비즈니스 로직 전역 상태(전역 변수) 금지 — 앱 공통 인프라 서비스(로거, 설정 등)는 `SageApp` 싱글톤으로 단일 관리 (`sageMgr` 매크로로 접근)
- UI 스레드에서 비즈니스 로직/통신 등 무거운 작업 금지
- 워커 스레드에서 View, MainFrame 직접 접근 금지
- 함수의 타입은 포인터로 작성 금지
- **`nullptr` 반환 금지** → 실패 시 예외(`SageException` 계열)를 던지거나, 출력 매개변수(`strError` 등)로 오류를 전달하고 호출자가 판단하도록 한다

### 네이밍 규칙
- **클래스명**: `C` 접두사 유지 (예: `CSAGEDashDoc`)
- **멤버 변수**: `m_` 접두사 + 헝가리안 표기법
- **함수 매개변수/지역변수**: 카멜 표기법 (camelCase)
- **상수**: 대문자 + `_` 구분 (예: `MAX_RETRY_COUNT`)
- **상수 선언**: `constexpr` 우선, MFC 레거시의 경우 `#define` 허용 (예: `#define WM_COMPLETE WM_USER + 1`)
- **함수명**: 동사 + 목적어 형태 (예: `LoadTemplate`, `ValidateMapping`)
- **클래스 이름**: 역할이 드러나야 함 (`MyDoc`, `Manager1` 같은 모호한 이름 금지)
- **bool/BOOL**: MFC 코드에서는 `bool` 대신 `BOOL` 타입을 사용한다. 변수명은 `is/has/can` 패턴 우선 (예: `isCompleted`, `hasError`)
- **함수 반환 타입**: 실패 가능한 함수는 `BOOL` 반환, 실패 이유가 필요하면 `CString& strError` 추가, 그 외 단순 동작은 `void`
- **`psz`, `lpsz`, `dw`, `lp`** 남발은 신규 코드에서 지양

### 컨트롤 변수 접두사
| 접두사 | 컨트롤 |
|--------|--------|
| `btn` | Button |
| `edt` | Edit |
| `stc` | Static |
| `lst` | List / ListCtrl |
| `cmb` | ComboBox |
| `chk` | CheckBox |
| `rdo` | RadioButton |
| `prg` | Progress |
| `dlg` | Dialog |

## 아키텍처 원칙

### 계층 분리
- **UI 계층 (MFC)**: Document/View/Frame은 UI 셸. 상태 표시 중심으로 사용
- **Application 계층**: 서비스/유스케이스/커맨드. 실제 업무 처리 로직 담당
- **Domain 계층**: 모델/규칙/정책. 핵심 비즈니스 규칙
- **Infrastructure 계층**: 엑셀 I/O, 파일시스템, 로깅, 설정 등 외부 연동

### 계층 의존 방향 (필수)
```
UI → Application → Domain
UI → Application → Infrastructure (through interface or service)
```
- **Domain은 UI/MFC를 참조하지 않는다**
- **Infrastructure는 Domain/Application 계약(인터페이스)을 구현한다**

### 핵심 설계 원칙
- UI와 비즈니스 로직을 분리한다
- 메시지 핸들러에서는 최대한 빠르게 서비스 계층으로 위임한다
- Document는 데이터 중심, View는 표현 중심, Frame은 레이아웃 중심
- 하나의 클래스는 하나의 책임에 집중한다
- 고객사별 차이는 템플릿/전략/확장 모듈로 분리한다
- 문서(Document)는 단순 파일이 아니라 **작업 세션의 데이터 모델**이다

## 스레드 규칙

- UI 스레드에서 무거운 작업 실행 금지
- 백그라운드 작업에서 UI 직접 접근 금지 → 메시지 전달/디스패치 방식으로 UI 갱신
- 문서 종료 시 해당 문서의 작업도 종료/취소/무효화
- 장시간 작업은 진행률과 취소 가능 여부를 제공
- 작업 취소 가능 구조를 우선 고려

## 예외 처리 규칙

- 예외/오류 코드는 가능한 한 **계층 경계**에서 잡는다
- 무분별한 try-catch 금지 → 클래스를 통해 예외 처리 관리
- 예외 클래스 네이밍: `Sage~` 접두사 (예: `SageException`)
- 추적 가능한 정보 포함 로그 남기기 (파일 경로, 템플릿 이름, 문서 ID 등)
- UI에는 이해 가능한 메시지를 보여준다
- 오류 로그 파일명: `yyyy-mm-dd` 형식
- 로그 포맷: `[yyyy-mm-dd HH:mm:ss.fff] ...` (연월일 시분초 밀리초)

## MFC 특화 규칙

- GDI 객체, 핸들, 파일 핸들 누수 방지
- 문서 종료 시 관련 작업/캐시 정리
- Pane/ChildFrame 소멸 시 이벤트 해제
- MFC 이벤트 핸들러 네이밍은 전통 유지 (예: `OnBnClickedOK`, `OnLvnItemChanged`)

## Git 규칙

### 브랜치 전략
- `main`: 배포 가능한 안정 버전
- `develop`: 통합 개발 브랜치
- `feature/*`: 기능 개발 (예: `feature/project-open`)
- `fix/*`: 버그 수정 (예: `fix/csv-encoding`)
- `refactor/*`: 구조 개선 (예: `refactor/template-service`)
- `docs/*`: 문서 작업

### 커밋 컨벤션
- 타입: `feat`, `fix`, `refactor`, `docs`, `test`, `chore`
- 한 커밋은 하나의 변경 목적을 가지도록 유지
- 의미 없는 대량 커밋 금지

### PR 규칙
- 작업 목적, 변경 범위 필수 기재
- UI 변경 시 스크린샷 포함
