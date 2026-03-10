
#pragma once

#include "Workbook.h"

// CWorkbookService: 파일로부터 워크북 로드를 담당하는 응용 서비스
class CWorkbookService
{
public:
    // 파일 경로를 받아 CWorkbook* 반환 (호출자가 소유권 가짐)
    // 실패 시 nullptr, 오류 메시지는 strError에 설정
    CWorkbook* LoadFromFile(const CString& strFilePath, CString& strError);

private:
    CWorkbook* LoadCsvFile(const CString& strFilePath, CString& strError);
};
