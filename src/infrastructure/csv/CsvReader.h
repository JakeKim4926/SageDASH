
#pragma once

#include "Workbook.h"

// CCsvReader: CSV 파일을 파싱하여 CWorksheet 반환
class CCsvReader
{
public:
    // 성공 시 CWorksheet* 반환 (호출자가 소유권 가짐), 실패 시 nullptr
    CWorksheet* ReadFile(const CString& strFilePath, CString& strError);

private:
    std::vector<CString> ParseLine(const CString& strLine);
};
