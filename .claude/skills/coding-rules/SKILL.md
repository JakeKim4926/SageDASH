---
name: coding-rules
description: C++ MFC 코드를 작성 및 수정하거나 리뷰할 때 반드시 사용한다.
auto 키워드, 스마트 포인터, nullptr 반환, 매직 넘버 등 금지 사항과
헝가리안 표기법, 클래스/함수/변수 네이밍 규칙을 적용한다.
코드 생성 및 수정, 리팩토링, 코드 리뷰, 함수 작성 요청 시 항상 이 스킬을 참고한다.
---

# C++ MFC 코딩 규칙

## 금지 사항

- `auto` 키워드 사용 금지
- 스마트 포인터 사용 금지
- 함수 길이 200줄 초과 금지
- 매직 넘버 금지 → `Define.h`에 도메인별 주석 블록을 먼저 표시하고, 해당 주석 아래에 그 도메인의 상수를 선언
- Enum은 `EnumDefine.h`에 선언
- 비즈니스 로직 전역 상태(전역 변수) 금지 — 앱 공통 인프라 서비스(로거, 설정 등)는 `SageApp` 싱글톤으로 단일 관리 (`sageMgr` 매크로로 접근)
- UI 스레드에서 비즈니스 로직/통신 등 무거운 작업 금지
- 워커 스레드에서 View, MainFrame 직접 접근 금지
- 함수의 반환 타입에 포인터 사용 금지
- `nullptr` 반환 금지 → 실패 시 예외(`SageException` 계열)를 던지거나, 출력 매개변수(`strError` 등)로 오류를 전달하고 호출자가 판단하도록 한다

## 네이밍 규칙

- **클래스명**: 영어 대문자로 시작하는 카멜케이스 (예: `SageDash`)
- **멤버 변수**: `m_` 접두사 + 헝가리안 표기법
- **함수 매개변수/지역변수**: 카멜 표기법 (camelCase)
- **상수**: 대문자 + `_` 구분 (예: `MAX_RETRY_COUNT`)
- **상수 선언**: `constexpr` 우선, MFC 레거시의 경우 `#define` 허용 (예: `#define WM_COMPLETE WM_USER + 1`)
- **함수명**: 동사 + 목적어 형태 (예: `LoadTemplate`, `ValidateMapping`)
- **클래스 이름**: 역할이 드러나야 함 (`MyDoc`, `Manager1` 같은 모호한 이름 금지)
- **bool/BOOL**: MFC 코드에서는 `bool` 대신 `BOOL` 타입을 사용한다. 변수명은 `is/has/can` 패턴 우선 (예: `isCompleted`, `hasError`)
- **함수 반환 타입**: 실패 가능한 함수는 `BOOL` 반환, 실패 이유가 필요하면 `CString& strError` 추가, 그 외 단순 동작은 `void`
- **`psz`, `lpsz`, `dw`, `lp`** 남발은 신규 코드에서 지양

## 컨트롤 변수 접두사

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
