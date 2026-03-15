
#pragma once

#include "Workbook.h"

// 실패 시 SageException을 던진다
class WorkbookService
{
public:
    void LoadFromFile(const CString& strFilePath, CWorkbook& outWorkbook);

private:
    void LoadCsvFile(const CString& strFilePath, CWorkbook& outWorkbook);
};
