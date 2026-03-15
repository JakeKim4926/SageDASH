# [이슈] 다국어 Satellite Resource DLL 구조 도입

## 배경

Phase 2에서 `LoadString(IDS_xxx)` 패턴을 적용했다.
현재는 RC 파일에 한국어 문자열만 존재하며 런타임 언어 전환이 불가능하다.

## 목표

- 런타임에 한/영 언어를 전환할 수 있어야 한다
- 새로운 언어 추가 시 C++ 코드 수정 없이 리소스 파일만 추가하면 된다
- MFC native 영역은 Satellite DLL 방식, WebView2 영역은 JSON 로케일 파일 방식으로 분리한다

## 설계 방향

### MFC Native — Satellite Resource DLL

```
SAGEDash.exe
SAGEDash.ko.dll    (한국어 리소스)
SAGEDash.en.dll    (영어 리소스)
```

- 언어별 `.rc` 파일을 각각 빌드해 DLL로 생성
- 앱 시작 시 설정(또는 시스템 로케일)에 따라 적절한 DLL 로드
- `AfxSetResourceHandle(hLangDll)` 한 줄로 전환
- 기존 `LoadString(IDS_xxx)` 호출 코드는 변경 없음

```cpp
// SageLangMgr (신규) 또는 SageMgr 확장
void SageLangMgr::LoadLanguage(const CString& strLangCode);
// 내부적으로 AfxSetResourceHandle() 호출
```

### WebView2 — JSON 로케일 파일

```
web/locale/ko.json
web/locale/en.json
```

- Phase 4 WebView2 파일럿 시 함께 구현

## 전제 조건

- 현재 RC의 STRINGTABLE을 한국어(LANGUAGE 18, 1) 블록으로 명시
- 영어용 `.rc` 파일 초안 작성 (IDS_ ID 동일, 문자열만 영어)
- 빌드 구성에 언어별 DLL 빌드 타겟 추가

## 작업 단위 (Phase 3 이후 진행)

- [ ] `SageLangMgr` 클래스 설계
- [ ] `SAGEDash.en.rc` 영어 문자열 초안 작성
- [ ] 언어별 DLL 빌드 설정
- [ ] 앱 설정에 언어 선택 항목 추가
- [ ] 재시작 없는 런타임 전환 (선택, Phase 6 이전에는 재시작 허용)
