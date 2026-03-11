
#pragma once

#include "Workbook.h"

// CCsvReader: CSV 파일을 파싱하여 outSheet 에 결과를 채운다
// 실패 시 SageException을 던진다
class CCsvReader
{
public:
    void ReadFile(const CString& strFilePath, CWorksheet& outSheet);

private:
    std::vector<CString> ParseLine(const CString& strLine);
};
