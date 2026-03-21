# PR 작업 로그

## [2026-03-21] feature/bridge-common
- **목적**: Phase 6-B — 브릿지 공통화 (메시지 타입 상수화 + JS 라우터 모듈 분리)
- **변경 내용**: WebBridgeMessage.h에 BRIDGE_TYPE_* constexpr 상수 4종 추가, WebBridgeMessage.cpp/SAGEDashView.cpp 문자열 리터럴 → 상수 교체, web/bridge.js 생성(SageBridge.on/send API + 메시지 라우터), dashboard.html 인라인 브릿지 코드 → bridge.js 방식으로 교체
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/41
- **결과**: merged into develop

## [2026-03-21] feature/batch-stability
- **목적**: Phase 6-A 완료 — 배치 실행 안정화 + 예외/로그/복구 흐름 정리
- **변경 내용**: BatchRunner 소멸자(DuplicateHandle + WaitForSingleObject 5s 안전 종료), ExecutionContext m_pCancelFlag + IsCancelled() 추가, MappingStep/ValidationStep/OutputStep 취소 플래그 체크, BatchRunner sageMgr.Log() 파일 로그 기록, BatchDialog 취소 중 UI 피드백(m_isCancelRequested + "Cancelling..." 버튼), OutputWnd 로그 연동, 하드코딩 문자열 String Table 전환(IDS 11209~11213), BATCH_SHUTDOWN_TIMEOUT_MS 상수 추가
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/39
- **결과**: merged into develop

## [2026-03-21] feature/action-step
- **목적**: Phase 5-C — ActionStep 추가 및 PipelineRunner 후속 액션 확장 (PR #33 충돌 우회 후 신규 코드만 추출)
- **변경 내용**: ActionStep(IActionHandler를 IExecutionStep으로 래핑), PipelineRunner::Run(ctx, arrExtra) 오버로드 추가, RunSteps() 헬퍼 함수 추출, vcxproj/filters 파일 등록
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/40
- **결과**: merged into develop

## [2026-03-20] feature/batch-dialog
- **목적**: Phase 6-A — 배치 실행 다이얼로그 구현 (BatchRunner UI 연결)
- **변경 내용**: BatchDialog(파일 목록/진행률/결과 로그/요약), SAGEDashDoc OnAutomationBatch, Resource.h/rc 배치 리소스, Automation 메뉴 항목 추가
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/38
- **결과**: merged into develop

## [2026-03-20] feature/batch-runner
- **목적**: Phase 6-A — 배치 실행 엔진 구현 (작업 큐 + 스레드 + 진행률 + 취소 + 집계)
- **변경 내용**: BatchJob/BatchResult/BatchSummary 도메인 모델, BatchRunner(AfxBeginThread 기반 워커 스레드), WM_BATCH_PROGRESS/JOB_DONE/COMPLETE 메시지 추가
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/37
- **결과**: merged into develop

## [2026-03-20] feature/project-action-serialization
- **목적**: Phase 5 완료 — AutomationProject에 후속 액션 직렬화 통합 (템플릿 기반 작업 정의 범위 확장)
- **변경 내용**: ActionType enum 추가, ActionDefinition 도메인 모델 신규, AutomationProject m_arrActions 필드 추가, ProjectSerializer [actions] 섹션 Save/Load, PROJECT_VERSION 1→2
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/36
- **결과**: merged into develop

## [2026-03-19] feature/ftp-upload
- **목적**: Phase 5 Step 9 (5-B) — FTP 업로드 시스템 업로드 액션 초안
- **변경 내용**: `FtpActionHandler` 구현 (WinInet FTP, InternetConnect/FtpPutFile, 임시 CSV 생성 후 업로드), `FtpUploadDialog` (Host/Port/User/Pass/RemoteDir/Filename), 메뉴·로그 문자열 추가
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/32
- **결과**: merged into develop

## [2026-03-19] feature/api-action
- **목적**: Phase 5 Step 8 (5-B) — HTTP/HTTPS API 전송 액션 추가
- **변경 내용**: `ApiActionHandler` 구현 (WinHTTP, URL 파싱, JSON 직렬화, POST/PUT, HTTP 상태 검증), `ApiSendDialog` (URL 입력 + Method 선택), 메뉴 항목·로그 문자열 추가
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/31
- **결과**: merged into develop

## [2026-03-19] feature/email-action
- **목적**: Phase 5 Step 7 (5-B) — 이메일 발송 액션 추가
- **변경 내용**: `IActionHandler` 인터페이스 신규 정의, `EmailActionHandler` 구현 (Simple MAPI, MAPI32.DLL 동적 로드, MAPISendMailW/MAPISendMail, 임시 CSV 첨부 후 메일 작성창 오픈), 메뉴 항목·로그 문자열 추가
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/30
- **결과**: merged into develop

## [2026-03-19] feature/pdf-output
- **목적**: Phase 5 Step 6 (5-B) — PDF 출력 추가
- **변경 내용**: `PdfOutputWriter` 신규 추가 (PDF 1.4 raw 직접 생성, 외부 라이브러리 없음, MalgunGothic/Identity-H, UTF-16BE 헥스, A4, 헤더 반복 페이지 분할), `ExportService` `.pdf` 분기, `IDS_EXPORT_FILE_FILTER`에 PDF 항목 추가
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/29
- **결과**: merged into develop

## [2026-03-19] feature/db-input
- **목적**: Phase 5 Step 5 — DB 조회 입력 초안 추가
- **변경 내용**: `DbInputReader` 신규 추가 (ODBC 기반, `.dbq` 파일 형식: 1행=연결문자열 2행~=쿼리, SQLDriverConnect → SQLExecDirect → SQLFetch), `WorkbookService` `.dbq` 분기, `odbc32.lib` 링크
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/28
- **결과**: merged into develop

## [2026-03-19] feature/folder-scan-input
- **목적**: Phase 5 Step 4 — 폴더 스캔 입력 추가
- **변경 내용**: `FolderScanInputReader` 신규 추가 (Win32 FindFirstFile/FindNextFile, 파일명/확장자/크기/날짜/전체경로 컬럼), `WorkbookService.LoadFromFolder`, `SAGEDashDoc.OnFileOpenFolder` (SHBrowseForFolder 다이얼로그), RC 파일 메뉴 항목 및 문자열 추가, vcxproj/filters 등록
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/27
- **결과**: pending

## [2026-03-18] feature/xml-input
- **목적**: Phase 5 Step 3 — XML 파일 입력 리더 추가
- **변경 내용**: `XmlInputReader` 신규 추가(속성/자식엘리먼트/혼합 방식, 주석·CDATA·DOCTYPE 건너뛰기, 엔티티 디코딩), `WorkbookService` .xml 분기, 파일 열기 필터 *.xml 추가, vcxproj 등록
- **PR 링크**: https://github.com/JakeKim4926/SageDASH/pull/26
- **결과**: merged into develop

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
