
#include "pch.h"
#include "framework.h"
#include "WorkbookService.h"
#include "CsvReader.h"
#include "SageException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWorkbook* CWorkbookService::LoadFromFile(const CString& strFilePath)
{
    int nDot = strFilePath.ReverseFind(_T('.'));
    if (nDot < 0)
        throw SageException(_T("파일 확장자를 확인할 수 없습니다."), strFilePath);

    CString strExt = strFilePath.Mid(nDot + 1);
    strExt.MakeLower();

    if (strExt == _T("csv"))
        return LoadCsvFile(strFilePath);

    CString strMsg;
    strMsg.Format(_T("지원하지 않는 파일 형식입니다: .%s"), (LPCTSTR)strExt);
    throw SageException(strMsg, strFilePath);
}

CWorkbook* CWorkbookService::LoadCsvFile(const CString& strFilePath)
{
    CCsvReader reader;
    CWorksheet* pSheet = reader.ReadFile(strFilePath); // 실패 시 SageException 전파

    CWorkbook* pWorkbook = new CWorkbook();
    pWorkbook->m_strFilePath = strFilePath;
    pWorkbook->m_arrSheets.push_back(pSheet);

    return pWorkbook;
}
