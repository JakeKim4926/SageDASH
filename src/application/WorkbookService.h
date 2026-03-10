
#pragma once

#include "Workbook.h"

// CWorkbookService: 파일로부터 워크북 로드를 담당하는 응용 서비스
// 실패 시 SageException을 던진다 (호출자가 소유권 가짐)
class CWorkbookService
{
public:
    CWorkbook* LoadFromFile(const CString& strFilePath);

private:
    CWorkbook* LoadCsvFile(const CString& strFilePath);
};
