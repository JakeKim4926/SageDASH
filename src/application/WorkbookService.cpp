
#include "pch.h"
#include "framework.h"
#include "WorkbookService.h"
#include "CsvReader.h"
#include "SageException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CWorkbookService::LoadFromFile(const CString& strFilePath, CWorkbook& outWorkbook)
{
    int nDot = strFilePath.ReverseFind(_T('.'));
    if (nDot < 0)
        throw SageException(_T("파일 확장자를 확인할 수 없습니다."), strFilePath);

    CString strExt = strFilePath.Mid(nDot + 1);
    strExt.MakeLower();

    if (strExt == _T("csv")) {
        LoadCsvFile(strFilePath, outWorkbook);
        return;
    }

    CString strMsg;
    strMsg.Format(_T("지원하지 않는 파일 형식입니다: .%s"), (LPCTSTR)strExt);
    throw SageException(strMsg, strFilePath);
}

void CWorkbookService::LoadCsvFile(const CString& strFilePath, CWorkbook& outWorkbook)
{
    outWorkbook.m_strFilePath = strFilePath;
    outWorkbook.m_arrSheets.emplace_back();

    CCsvReader reader;
    reader.ReadFile(strFilePath, outWorkbook.m_arrSheets.back());
}
