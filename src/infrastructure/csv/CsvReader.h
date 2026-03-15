
#pragma once

#include "Workbook.h"

// 실패 시 SageException을 던진다
class CSVReader
{
public:
    void ReadFile(const CString& strFilePath, CWorksheet& outSheet);

private:
    std::vector<CString> ParseLine(const CString& strLine);
};
