# PR 작업 로그

## [2026-03-18] feature/json-input
- **목적**: Phase 5 Step 2 — JSON 파일 입력 리더 추가
- **변경 내용**: `JsonInputReader` 신규 추가(객체 배열/배열의 배열 지원, UTF-8 BOM 처리), `WorkbookService` .json 분기, 파일 열기 필터 *.json 추가, vcxproj 등록
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/25
- **결과**: merged into develop

## [2026-03-18] feature/pipeline-runner
- **목적**: Phase 5 Step 1 — 파이프라인 실행 엔진 구현
- **변경 내용**: InputStep/MappingStep/ValidationStep/OutputStep 구현, PipelineRunner 추가, 자동화 메뉴(F5) 추가
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/24
- **결과**: merged into develop

## [2026-03-18] fix/navigator-file-item
- **목적**: Navigator File 항목 클릭 무반응 버그 수정 + 현재 뷰 모드 시각적 표시
- **변경 내용**: `m_hFile` 핸들 저장, `UpdateFileItem()`/`SetActiveMode()` 추가, `NAV_ITEM_CURRENT` 타입 신규 도입, 뷰 전환 시 Navigator 하이라이트 갱신
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/23
- **결과**: merged into develop

## [2026-03-17] fix/csv-delimiter-autodetect
- **목적**: 탭 구분자 CSV 파일 열면 한 열로 뭉치는 문제 수정
- **변경 내용**: `CsvInputReader`에 첫 줄 기준 구분자 자동 감지(탭/쉼표/세미콜론) 추가, `Define.h`에 CSV 구분자 상수 추가
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/22
- **결과**: merged into develop

## [2026-03-17] fix/suppress-startup-empty-doc
- **목적**: 앱 시작 시 MFC 기본 동작으로 인해 빈 문서(SAGEDash1)가 자동 생성되는 문제 수정
- **변경 내용**: `SAGEDash.cpp` — `ParseCommandLine` 후 `FileNew` 명령을 `FileNothing`으로 변경
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/21
- **결과**: merged into develop

## [2026-03-17] fix/grid-duplicate-rows-on-load
- **목적**: 파일 로드 시 빈 행 중복 표시 버그 수정
- **변경 내용**: OnEnChangeSearch에 컬럼 없을 때 조기 반환 가드 추가
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/18
- **결과**: merged into develop

## [2026-03-17] refactor/constants-to-define-h
- **목적**: coding-rules 위반 수정 — .cpp에 분산된 상수를 Define.h 도메인 블록으로 통합
- **변경 내용**: MappingPanel/ValidationPanel/SAGEDashView/PropertiesPane의 IDC_* + 레이아웃 constexpr 상수 전부 Define.h로 이동, Define.h vcxproj 등록
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/17
- **결과**: merged into develop

## [2026-03-17] feature/webview2-dashboard
- **목적**: Phase 4-7 — 대시보드 웹 화면 구현 + Phase 3 데이터 실제 연동
- **변경 내용**: WebViewHost::Navigate() 추가, NavigatorPane Dashboard 항목 추가, SAGEDashView VIEW_MODE_DASHBOARD 진입 시 file:// URL 탐색, web:request-summary → data:summary 브릿지 응답, web/dashboard.html (KPI 카드 + Summary 테이블), vcxproj CopyWebAssets 빌드 타겟, .gitattributes .rc binary 처리
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/16
- **결과**: merged into develop

## [2026-03-17] feature/webview2-bridge-protocol-doc
- **목적**: Phase 4-6 — JSON 메시지 규약 최종 확정 및 문서화
- **변경 내용**: WebBridgeMessage::ParseType() 추가, DECISION_webview2_bridge_protocol.md 완성 (Web→C++ 섹션, JS 연동 템플릿, C++ dispatch 패턴)
- **PR 링크**: (pending)
- **결과**: pending

## [2026-03-17] feature/webview2-bridge-cpp-to-web
- **목적**: Phase 4-4 — C++ → Web 단방향 브릿지 구현
- **변경 내용**: WebBridgeMessage 빌더 클래스 추가, WebViewHost PostWebMessageAsJson LPCWSTR 캐스트 수정, 초기화 완료 시 native:ready 자동 발송, DECISION_webview2_bridge_protocol.md 작성
- **PR 링크**: (pending)
- **결과**: pending

## [2026-03-17] feature/webview2-setup
- **목적**: Phase 4-1 — WebView2 NuGet 패키지 빌드 환경 세팅
- **변경 내용**: packages.config 추가, vcxproj ExtensionSettings/ExtensionTargets에 WebView2 props/targets import, EnsureNuGetPackagesBuildImports 타겟으로 복원 누락 시 빌드 오류 처리, DECISION_webview2_setup.md 작성
- **PR 링크**: (pending)
- **결과**: pending

## [2026-03-16] refactor/naming-rules-fix
- **목적**: coding-rules 위반 전수 수정 — 클래스명 C 접두사 제거, 매직 넘버 상수화, 포인터 반환 제거
- **변경 내용**: CNavigatorPane/CPropertiesPane/COutputWnd → C 접두사 제거, GetActiveView() BOOL+출력 파라미터로 변경, MappingPanel/ValidationPanel/PropertiesPane 레이아웃 매직 넘버 → constexpr 상수화, Define.h 공통 상수 추가, coding-rules skill description 업데이트
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/9
- **결과**: merged into develop

## [2026-03-16] develop → main (Phase 3 완료)
- **목적**: Phase 3 완료 버전 main 반영 — 자동화 핵심 기능(매핑/검증/출력/프로젝트)
- **변경 내용**: Phase 3 전체 — Center View 모드 전환, MappingPanel, ValidationPanel, ValidationService, CsvOutputWriter, ExcelOutputWriter, ExportService, IExecutionStep/ExecutionContext, AutomationProject, ProjectSerializer, ProjectService, 빌드 오류 수정(forward declaration, include 경로)
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/8
- **결과**: merged into main

## [2026-03-16] feature/phase3-output
- **목적**: 결과 출력 — CSV/XLSX 저장 + 출력 로그 기록
- **변경 내용**: CsvOutputWriter(UTF-8 BOM, RFC 4180 이스케이프), ExcelOutputWriter(COM automation), FileOutputTarget, ExportService(확장자 기반 writer 선택), SAGEDashDoc OnFileExport 핸들러(SaveFileDialog→ExportService→로그), File 메뉴 내보내기 항목 추가
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/6
- **결과**: merged into develop

## [2026-03-16] feature/phase3-validation-panel
- **목적**: Validation Panel 구현 — 검증 규칙(필수값/포맷) 등록 + 데이터 검증 실행 + 오류/경고 결과 표시
- **변경 내용**: ValidationService 신규 추가(Application 계층), ValidationPanel 전체 구현(컬럼/타입/심각도/패턴 입력, 추가/삭제/전체삭제, 검증 실행, 결과 리스트 오류·경고 색상), SAGEDashView Validation 모드 전환 시 SetSourceColumns+SetDataSheet 주입, IDS_VIEW_VAL_* 10316~10334 추가
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/5
- **결과**: merged into develop

## [2026-03-16] feature/phase3-mapping-required
- **목적**: 매핑 패널 필수 항목 누락 표시 — 어떤 컬럼이 매핑 안 됐는지 시각적으로 표시
- **변경 내용**: MappingPanel에 status 영역 추가(매핑: X/N | 미매핑: Y개, 색상 변화), 필수 규칙 행 COLOR_ACCENT_LIGHT 하이라이트(NM_CUSTOMDRAW), IDS_VIEW_MAPPING_STATUS_FMT/ALL_OK 추가
- **PR 링크**: (gh CLI 미설치로 직접 머지)
- **결과**: merged into develop

## [2026-03-16] feature/phase3-mapping-panel
- **목적**: Phase 3 Mapping Panel 구현 — Source/Target 컬럼 매핑 UI + Navigator Pipeline 활성화
- **변경 내용**: MappingPanel 전체 구현(ComboBox/Edit/Checkbox/Add/Delete/AutoMap/Clear/CListCtrl), SAGEDashView에서 Mapping 모드 전환 시 Source 컬럼 주입, NavigatorPane Pipeline 항목 활성화/클릭 → 뷰 전환, SAGEDashDoc 로드/클리어 시 Navigator 상태 연동, IDS_ 10304~10313 String Table 추가
- **PR 링크**: (gh CLI 미설치로 직접 머지)
- **결과**: merged into develop

## [2026-03-15] feature/phase3-view-switching
- **목적**: Center View 모드 전환 구조 구축 — Grid / Mapping / Validation 세 가지 뷰 모드 간 전환 인프라
- **변경 내용**: CenterViewMode enum 추가, MappingPanel/ValidationPanel placeholder CWnd 생성, CSAGEDashView에 SwitchViewMode 구현, MainFrame에 WM_SWITCH_CENTER_VIEW 핸들러 추가, IDS_VIEW_MAPPING_TITLE/IDS_VIEW_VALIDATION_TITLE 문자열 추가
- **PR 링크**: (gh CLI 미설치로 직접 머지)
- **결과**: merged into develop

## [2026-03-15] feature/phase3-domain-model
- **목적**: Phase 3 자동화 핵심 기능의 도메인 모델 기반 구축
- **변경 내용**: MappingRule, ValidationRule, ValidationResult/ValidationIssue 헤더 신규 추가. EnumDefine.h에 ExecutionStepType/ValidationType/ValidationSeverity enum 추가
- **PR 링크**: (gh CLI 미설치로 직접 머지)
- **결과**: merged into develop

## [2026-03-15] feature/phase2-ui
- **목적**: Phase 2 UI 활성화 — Navigator 전체 구조, Properties Summary 카드, 검색/필터, 다국어 String Table 기반 구조 적용
- **변경 내용**: Navigator INPUT/PIPELINE/ACTIONS 섹션 추가 + NM_CUSTOMDRAW 색상 구분, Properties FILE/DATA/SHEETS 섹션 카드화 + XLSX 시트 정보 표시, View 상단 실시간 검색/필터, IDS_ 상수 + RC STRINGTABLE + LoadString 패턴 적용
- **PR 링크**: (gh CLI 미설치로 직접 머지)
- **결과**: merged into develop
- **이월**: Satellite Resource DLL 구조 도입 (런타임 한/영 전환 및 다국어 확장) → 별도 이슈로 관리

## [2026-03-15] refactor/phase2-structure
- **목적**: Phase 2 구조 안정화 + 코어 일반화 — UI와 업무 로직 분리, 확장 가능한 뼈대 구성
- **변경 내용**: 레이어 분리(application/domain/infrastructure), IInputReader/IInputSource/IOutputWriter/IOutputTarget 인터페이스 정의, CsvInputReader/ExcelInputReader 구현체 분리, TabularData 코어 모델 일반화, Workbook.h·CsvReader 제거, 데이터 흐름 File Open→WorkbookService→Reader→Doc→View 확립
- **PR 링크**: (gh CLI 미설치로 직접 머지)
- **결과**: merged into develop
- **이월**: Navigator 전체 활성화, Properties Summary 카드, XLSX 관련 표시, 검색/필터, 다국어 String Table 적용 (Phase 2 UI 작업)

## [2026-03-15] feature/phase1-ui-panels
- **목적**: sagedash-ui 스펙 기준 Phase 1 누락 UI 갭 해소
- **변경 내용**: Navigator/Properties Pane 신규 추가, COLOR_* 색상 토큰 정의, 최소 윈도우 크기 800×600 적용
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/1
- **결과**: merged

## [2026-03-15] feature/phase1-error-handling
- **목적**: Phase 1 오류 처리 안정화 (1-4) 구현
- **변경 내용**: 에러 다이얼로그, 헤더 전용 파일 경고, Properties Pane 연동, CSV 인코딩 결정 문서화
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/2
- **결과**: merged

## [2026-03-15] fix/pane-visibility
- **목적**: Navigator/Properties Pane VS2026 미표시 수정, vcxproj.filters stale 항목 정리
- **변경 내용**: SetRegistryKey 수정, View 메뉴 토글 핸들러 추가, filters 정리 (ViewTree/ClassView/FileView/PropertiesWnd 항목 제거)
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/4
- **결과**: merged

## [2026-03-15] develop → main (Phase 1 완료)
- **목적**: Phase 1 완료 버전 main 반영
- **변경 내용**: PR #1, #2 포함 — Navigator/Properties Pane, 오류 처리, 색상 토큰, CSV 인코딩 결정
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/3
- **결과**: merged

## [2026-03-16] feature/phase3-project
- **목적**: Phase 3-B 실행 구조 일반화 — 프로젝트/템플릿 저장/불러오기
- **변경 내용**: IExecutionStep/ExecutionContext(파이프라인 추상화), AutomationProject(규칙 집합 모델), ProjectSerializer(.sagep INI 형식), ProjectService(저장/로드), MappingPanel/ValidationPanel LoadRules() 추가, SAGEDashDoc 프로젝트 저장(&P)/열기(&L) 메뉴 핸들러
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/7
- **결과**: merged into develop
