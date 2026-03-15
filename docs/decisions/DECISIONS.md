# 의사결정 로그

프로젝트에서 내린 주요 설계/기술 결정을 기록한다.
한 번 결정된 사항은 명시적으로 번복하지 않는 한 유지된다.

---

## [2026-03-15] CSV 파일 인코딩 처리는 Phase 2로 이월

**결정**: Phase 1의 `CsvReader`는 UTF-8 BOM 제거만 처리하고, 완전한 인코딩 지원은 Phase 2에서 구현한다.

**배경**:
- `CStdioFile`을 `typeText` 모드로 열면 Windows CRT가 시스템 코드페이지(CP949) 기준으로 바이트를 읽는다.
- UTF-8 BOM(EF BB BF)은 현재 코드에서 제거되지만, 한글 데이터가 UTF-8로 인코딩된 경우 실제 내용은 깨진다.
- CSV는 인코딩 표준이 없어 실무에서는 ANSI/CP949, UTF-8 with BOM, UTF-8 without BOM, UTF-16 LE 등 다양하게 혼재한다.

**Phase 1 허용 범위**: ASCII 또는 ANSI/CP949 CSV 파일.

**Phase 2 구현 방향**:
- 파일 열기 시 BOM 바이트를 먼저 읽어 인코딩 자동 감지
- UTF-8 파일은 `MultiByteToWideChar(CP_UTF8, ...)` 로 변환 후 파싱
- CP949 파일은 `MultiByteToWideChar(CP_ACP, ...)` 또는 `CFile::modeRead | CFile::typeBinary` 후 변환
- 감지 실패 시 사용자에게 인코딩 선택 UI 제공 검토
