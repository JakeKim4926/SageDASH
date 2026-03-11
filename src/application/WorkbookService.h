
#pragma once

#include "Workbook.h"

// CWorkbookService: 파일로부터 워크북 로드를 담당하는 응용 서비스
// 실패 시 SageException을 던진다
class CWorkbookService
{
public:
    void LoadFromFile(const CString& strFilePath, CWorkbook& outWorkbook);

private:
    void LoadCsvFile(const CString& strFilePath, CWorkbook& outWorkbook);
};
