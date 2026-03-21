# SAGEDash 개발자 가이드

## 환경 설정

- Visual Studio 2022 이상
- Windows SDK 10.0.22000 이상
- NuGet 패키지: Microsoft.Web.WebView2
- 빌드 구성: x64 Debug / x64 Release

### 빌드 후 웹 자산 자동 복사
프로젝트 속성 → 빌드 이벤트 → 빌드 후 이벤트에 추가:
```
xcopy /E /Y /I "$(SolutionDir)web" "$(OutDir)web"
xcopy /E /Y /I "$(SolutionDir)templates" "$(OutDir)templates"
```

---

## 새 입력 커넥터 추가

예: `DbInputReader` (DB 조회 입력)

1. `src/infrastructure/db/DbInputReader.h/.cpp` 생성
2. `IInputReader` 인터페이스 구현:
   ```cpp
   class DbInputReader : public IInputReader {
   public:
       TabularData Read(const CString& strConnectionString);
   };
   ```
3. `WorkbookService::OpenFile()`에 확장자/타입 분기 추가
4. `vcxproj`에 파일 등록

---

## 새 파이프라인 단계 추가

예: `DeduplicateStep` (중복 행 제거)

1. `src/application/pipeline/DeduplicateStep.h/.cpp` 생성
2. `IExecutionStep` 구현:
   ```cpp
   class DeduplicateStep : public IExecutionStep {
   public:
       CString GetName() const override;
       BOOL    Execute(ExecutionContext& ctx) override;
   };
   ```
3. `PipelineRunner::Run(ctx, arrExtra)` 오버로드에 추가하거나,
   호출부에서 직접 `arrExtra`에 포함시킨다
4. `ExecutionContext`에 단계별 결과가 필요하면 필드 추가

---

## 새 출력 포맷 추가

예: `JsonOutputWriter`

1. `src/infrastructure/json/JsonOutputWriter.h/.cpp` 생성
2. `IOutputWriter` 인터페이스 구현
3. `OutputStep::Execute()`에서 출력 경로 확장자 기반으로 분기
4. Export 파일 다이얼로그 필터(`IDS_EXPORT_FILE_FILTER`)에 확장자 추가

---

## 새 WebView2 화면 추가

예: `trends.html` (트렌드 분석)

1. `web/trends.html` 생성 — `style.css`, `bridge.js` 로드
   ```html
   <link rel="stylesheet" href="style.css">
   <script src="bridge.js"></script>
   ```
2. `EnumDefine.h`에 `VIEW_MODE_TRENDS = N` 추가
3. `WebBridgeMessage.h`에 새 브릿지 타입 상수 추가:
   ```cpp
   constexpr LPCTSTR BRIDGE_TYPE_TRENDS_DATA = _T("data:trends");
   constexpr LPCTSTR BRIDGE_TYPE_WEB_REQUEST_TRENDS = _T("web:request-trends");
   ```
4. `WebBridgeMessage.cpp`에 `BuildTrendsData(...)` 빌더 추가
5. `Resource.h`에 `IDS_NAV_ITEM_TRENDS = 10214` 추가
6. `SAGEDash.rc` String Table에 항목 추가
7. `NavigatorPane.h/.cpp`에 `m_hTrends` 항목 추가
8. `SAGEDashView.cpp`의 `SwitchViewMode`, `UpdateLayout`, `OnWebBridgeMessage` 확장

---

## 새 후속 액션 추가

예: `SlackActionHandler`

1. `EnumDefine.h`에 `ACTION_SLACK = 3` 추가
2. `src/infrastructure/action/SlackActionHandler.h/.cpp` 생성
3. `IActionHandler` 구현:
   ```cpp
   class SlackActionHandler : public IActionHandler {
   public:
       BOOL Execute(const TabularData& data) override;
   };
   ```
4. `ActionDefinition`에 Slack 설정 필드 추가 (webhook URL 등)
5. `ProjectSerializer`에 직렬화/역직렬화 추가
6. `SAGEDashDoc`에서 `OnFileSlackAction()` 메뉴 핸들러 추가

---

## 사용자 메시지 추가 규칙

모든 사용자 노출 문자열은 String Table에 등록한다.

| 범위 | ID 범위 |
|------|---------|
| 메뉴 | 10001~10099 |
| 툴바 | 10101~10199 |
| 네비게이터 | 10201~10299 |
| View | 10301~10399 |
| Properties | 10401~10499 |
| 로그 메시지 | 10501~10599 |
| 공통 에러 | 12001~ |

```cpp
CString str;
str.LoadString(IDS_ERR_FILE_OPEN);
CString msg;
msg.Format(str, (LPCTSTR)strFilePath);
throw SageException(msg);
```

---

## 로그 작성 규칙

```cpp
sageMgr.Log(_T("[ClassName] 동작 설명 — 결과"));
```

- `sageMgr.Log()`는 스레드 안전(CCriticalSection 보호)
- 로그 파일: `{exe}/logs/yyyy/mm/yyyy-mm-dd.log`
- 포맷: `[yyyy-mm-dd HH:mm:ss.fff] 메시지`
- 워커 스레드에서도 직접 호출 가능

---

## 템플릿 파일 (.sagep)

기본 템플릿은 `templates/` 폴더에 있으며, 앱 배포 시 함께 복사된다.
`File > Open Project`에서 불러오거나, 앞으로 "최근 템플릿" 기능으로 연결 예정.

포맷 명세는 `ProjectSerializer.cpp`의 INI 섹션 구조를 참조한다.
