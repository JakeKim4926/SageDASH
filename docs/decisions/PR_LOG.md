# PR 작업 로그

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
