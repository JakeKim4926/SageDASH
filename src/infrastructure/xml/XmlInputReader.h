
#pragma once

#include "IInputReader.h"

// ============================================================
// XmlInputReader — XML 파일을 TabularData로 변환 (Phase 5)
//
// 지원 형식:
//   1) 레코드 목록
//      <root>
//        <row col1="v1" col2="v2" />
//        ...
//      </root>
//
//   2) 속성 대신 자식 엘리먼트로 값 표현
//      <root>
//        <row>
//          <col1>v1</col1>
//          <col2>v2</col2>
//        </row>
//        ...
//      </root>
//
//   3) 혼합 (속성 + 자식 엘리먼트 — 속성을 먼저 읽고 자식 엘리먼트를 이어 읽음)
//
// 루트 엘리먼트 이름은 무관. 루트의 직계 자식 엘리먼트를 행으로 처리.
// 첫 번째 행에서 컬럼 이름을 결정.
// ============================================================
class XmlInputReader : public IInputReader
{
public:
    void Read(const CString& strFilePath, TabularData& outData) override;
};
