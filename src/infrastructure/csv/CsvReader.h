
#pragma once

#include "Workbook.h"

// CCsvReader: CSV 파일을 파싱하여 CWorksheet 반환
// 실패 시 SageException을 던진다 (호출자가 소유권 가짐)
class CCsvReader
{
public:
    CWorksheet* ReadFile(const CString& strFilePath);

private:
    std::vector<CString> ParseLine(const CString& strLine);
};
