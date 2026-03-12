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
