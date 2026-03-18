
#pragma once

#include "IInputReader.h"

// ============================================================
// JsonInputReader — JSON 파일을 TabularData로 변환 (Phase 5)
//
// 지원 형식:
//   1) 객체 배열  [ {"col1": "v1", "col2": "v2"}, ... ]
//   2) 배열의 배열 [ ["col1", "col2"], ["v1", "v2"], ... ]
// ============================================================
class JsonInputReader : public IInputReader
{
public:
    void Read(const CString& strFilePath, TabularData& outData) override;
};
