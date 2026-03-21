# SAGEDash 아키텍처 개요

## 레이어 구조

```
┌──────────────────────────────────────────────────┐
│  UI Layer (MFC)                                  │
│  CMainFrame / CSAGEDashDoc / CSAGEDashView       │
│  Pane (Navigator, Properties, Output)            │
│  Dialog (Batch, API, FTP)                        │
│  WebViewHost (WebView2 호스트)                   │
├──────────────────────────────────────────────────┤
│  Application Layer                               │
│  WorkbookService — 파일 열기 코디네이터           │
│  PipelineRunner  — 4+N 단계 파이프라인 실행기     │
│  BatchRunner     — 다중 파일 배치 실행 (워커 스레드)│
│  pipeline/       — InputStep, MappingStep,       │
│                    ValidationStep, OutputStep,   │
│                    ActionStep                    │
├──────────────────────────────────────────────────┤
│  Domain Layer                                    │
│  AutomationProject / ExecutionContext            │
│  TabularData / DataSheet                         │
│  MappingRule / ValidationRule / ActionDefinition │
│  ValidationResult / ExecutionResult              │
│  IInputReader / IOutputWriter / IActionHandler   │
│  IExecutionStep                                  │
├──────────────────────────────────────────────────┤
│  Infrastructure Layer                            │
│  csv/    — CsvInputReader, CsvOutputWriter       │
│  excel/  — ExcelInputReader, ExcelOutputWriter   │
│  json/   — JsonInputReader                       │
│  xml/    — XmlInputReader                        │
│  pdf/    — PdfOutputWriter                       │
│  filesystem/ — FolderScanInputReader             │
│  action/ — EmailActionHandler, ApiActionHandler, │
│             FtpActionHandler                     │
│  persistence/ — ProjectSerializer (.sagep)       │
│  logging/ — SageLogger                          │
└──────────────────────────────────────────────────┘
```

## 계층 의존 방향

```
UI → Application → Domain
UI → Application → Infrastructure (인터페이스를 통해)
Infrastructure → Domain (인터페이스 구현)
```

- Domain은 MFC/UI에 의존하지 않는다
- Infrastructure는 Domain 인터페이스를 구현한다
- Application은 Infrastructure를 직접 알지 않고 인터페이스로 접근한다

---

## 핵심 데이터 흐름

### 파일 열기
```
CSAGEDashDoc::OnOpenDocument()
  → WorkbookService::OpenFile(strPath)
    → CsvInputReader / ExcelInputReader / JsonInputReader / XmlInputReader
    → TabularData (DataSheet 배열)
  → CSAGEDashDoc 저장
  → CSAGEDashView::OnUpdate() → Grid 표시
```

### 파이프라인 실행 (단일)
```
CSAGEDashDoc::OnPipelineRun()
  → PipelineRunner::Run(ctx)
    → InputStep   : ctx.m_pSourceSheet 설정
    → MappingStep : 컬럼 매핑 → ctx.m_transformedSheet
    → ValidationStep : 규칙 검증 → ctx.m_validationResult
    → OutputStep  : CSV/XLSX 저장
  → OutputPane 로그 출력
```

### 배치 실행
```
CSAGEDashDoc::OnAutomationBatch()
  → BatchDialog::DoModal()
    → BatchRunner::Start(arrFiles, project)
      → 워커 스레드: PipelineRunner::Run() × N
      → PostMessage(WM_BATCH_COMPLETE, result)
    → BatchDialog UI 갱신 (진행률 / 로그)
```

### WebView2 브릿지
```
C++ → Web:
  WebViewHost::PostWebMessage(strJson)
  ← WebBridgeMessage::Build*(...)

Web → C++:
  WebViewHost → WM_WEBBRIDGE_MESSAGE → CSAGEDashView::OnWebBridgeMessage()
  ← WebBridgeMessage::ParseType(strJson)
  → 타입별 분기 → WebBridgeMessage::Build*() → PostWebMessage()
```

---

## 주요 클래스 일람

| 클래스 | 위치 | 역할 |
|--------|------|------|
| `CSAGEDashApp` | `src/app/` | MFC 앱 진입점, SageMgr 초기화 |
| `CMainFrame` | `src/ui/frame/` | MDI 프레임, 도킹 패널 관리 |
| `CSAGEDashDoc` | `src/ui/doc/` | 문서 (데이터 + 프로젝트 상태) |
| `CSAGEDashView` | `src/ui/view/` | Center View (Grid/Mapping/Validation/WebView 전환) |
| `NavigatorPane` | `src/ui/pane/` | 좌측 네비게이터 트리 |
| `OutputPane` | `src/ui/pane/` | 하단 로그 출력 |
| `WorkbookService` | `src/application/` | 파일 열기 코디네이터 |
| `PipelineRunner` | `src/application/` | 자동화 파이프라인 실행기 |
| `BatchRunner` | `src/application/` | 다중 파일 배치 실행 |
| `ExecutionContext` | `src/domain/` | 파이프라인 단계 간 공유 상태 |
| `AutomationProject` | `src/domain/` | 매핑/검증 규칙 모음 |
| `TabularData` | `src/domain/` | 행/열 데이터 컨테이너 |
| `WebViewHost` | `src/ui/webview/` | WebView2 호스트 래퍼 |
| `WebBridgeMessage` | `src/ui/webview/` | 브릿지 메시지 빌더/파서 |
| `SageMgr` | `src/common/` | 싱글톤 앱 서비스 (`sageMgr` 매크로) |
| `SageLogger` | `src/infrastructure/logging/` | 날짜별 로그 파일 기록 |
| `ProjectSerializer` | `src/infrastructure/persistence/` | `.sagep` 프로젝트 파일 직렬화 |

---

## 파일 포맷

| 포맷 | 확장자 | 용도 |
|------|--------|------|
| CSV | `.csv` | 입력/출력 |
| Excel | `.xlsx` | 입력/출력 |
| JSON | `.json` | 입력 |
| XML | `.xml` | 입력 |
| PDF | `.pdf` | 출력 |
| 프로젝트 | `.sagep` | 매핑/검증 규칙 저장 (INI 형식) |
| 로그 | `.log` | `logs/yyyy/mm/yyyy-mm-dd.log` |

---

## WebView2 화면 목록

| 화면 | HTML | 모드 | 브릿지 메시지 |
|------|------|------|--------------|
| Dashboard | `web/dashboard.html` | `VIEW_MODE_DASHBOARD` | `data:summary` |
| Report | `web/report.html` | `VIEW_MODE_REPORT` | `data:validation-report` |
| Analysis | `web/analysis.html` | `VIEW_MODE_ANALYSIS` | `data:column-analysis` |

공통 모듈: `web/bridge.js` (SageBridge.on/send), `web/style.css` (디자인 시스템)
