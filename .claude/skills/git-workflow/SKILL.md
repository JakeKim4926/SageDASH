---
name: git-workflow
description: 브랜치 생성, 커밋, PR 작성 등 Git 작업을 할 때 반드시 사용한다.
브랜치 이름 제안, 커밋 메시지 작성, PR 내용 작성, 브랜치 전략 확인 요청 시 항상 이 스킬을 참고한다.
SAGEDash 프로젝트의 Git 컨벤션을 따른다.
---

# Git Workflow 규칙

## 브랜치 전략

| 브랜치 | 용도 | 예시 |
|--------|------|------|
| `main` | 배포 가능한 안정 버전 | — |
| `develop` | 통합 개발 브랜치 | — |
| `feature/*` | 기능 개발 | `feature/project-open` |
| `fix/*` | 버그 수정 | `fix/csv-encoding` |
| `refactor/*` | 구조 개선 | `refactor/template-service` |
| `docs/*` | 문서 작업 | `docs/architecture-guide` |

## 커밋 컨벤션

### 타입
- `feat` : 새 기능
- `fix` : 버그 수정
- `refactor` : 동작 변경 없는 구조 개선
- `docs` : 문서 작업
- `test` : 테스트 추가/수정
- `chore` : 빌드, 설정 등 기타

### 규칙
- 한 커밋은 **하나의 변경 목적**만 가진다
- 의미 없는 대량 커밋 금지
- 커밋 메시지 형식: `타입: 한 줄 요약`

**예시:**
```
feat: 프로젝트 열기 다이얼로그 추가
fix: CSV 인코딩 오류 수정
refactor: TemplateService 계층 분리
docs: 아키텍처 문서 초안 작성
```

## PR 규칙

PR 작성 시 아래 항목을 필수 기재한다:

```
## 작업 목적
(이 PR이 왜 필요한지)

## 변경 범위
(어떤 파일/계층이 변경되었는지)

## 스크린샷
(UI 변경이 있는 경우 필수)

## 참고 사항
(리뷰어가 알아야 할 내용, 있으면 기재)
```

## PR 작업 로그

PR을 생성하거나 머지할 때마다 `docs/decisions/PR_LOG.md`에 아래 형식으로 기록한다.

```
## [yyyy-mm-dd] 브랜치명
- **목적**: 무엇을 위한 작업인지
- **변경 내용**: 주요 작업 요약
- **PR 링크**: (있으면)
- **결과**: merged / closed / pending
```
