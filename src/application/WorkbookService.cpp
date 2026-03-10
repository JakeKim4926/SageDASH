
#include "pch.h"
#include "framework.h"
#include "WorkbookService.h"
#include "CsvReader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWorkbook* CWorkbookService::LoadFromFile(const CString& strFilePath, CString& strError)
{
    int nDot = strFilePath.ReverseFind(_T('.'));
    if (nDot < 0) {
        strError = _T("파일 확장자를 확인할 수 없습니다.");
        return nullptr;
    }

    CString strExt = strFilePath.Mid(nDot + 1);
    strExt.MakeLower();

    if (strExt == _T("csv"))
        return LoadCsvFile(strFilePath, strError);

    strError.Format(_T("지원하지 않는 파일 형식입니다: .%s"), (LPCTSTR)strExt);
    return nullptr;
}

CWorkbook* CWorkbookService::LoadCsvFile(const CString& strFilePath, CString& strError)
{
    CCsvReader reader;
    CWorksheet* pSheet = reader.ReadFile(strFilePath, strError);
    if (pSheet == nullptr)
        return nullptr;

    CWorkbook* pWorkbook = new CWorkbook();
    pWorkbook->m_strFilePath = strFilePath;
    pWorkbook->m_arrSheets.Add(pSheet);

    return pWorkbook;
}
