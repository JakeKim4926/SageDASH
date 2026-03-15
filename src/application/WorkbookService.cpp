
#include "pch.h"
#include "framework.h"
#include "WorkbookService.h"
#include "IInputReader.h"
#include "CsvInputReader.h"
#include "ExcelInputReader.h"
#include "SageException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void WorkbookService::LoadFromFile(const CString& strFilePath, CTabularData& outData)
{
    int nDot = strFilePath.ReverseFind(_T('.'));
    if (nDot < 0)
        throw SageException(_T("파일 확장자를 확인할 수 없습니다."), strFilePath);

    CString strExt = strFilePath.Mid(nDot + 1);
    strExt.MakeLower();

    auto reader = CreateReader(strExt);
    reader->Read(strFilePath, outData);
}

std::unique_ptr<IInputReader> WorkbookService::CreateReader(const CString& strExt)
{
    if (strExt == _T("csv"))
        return std::make_unique<CsvInputReader>();

    if (strExt == _T("xlsx") || strExt == _T("xls"))
        return std::make_unique<ExcelInputReader>();

    CString strMsg;
    strMsg.Format(_T("지원하지 않는 파일 형식입니다: .%s"), (LPCTSTR)strExt);
    throw SageException(strMsg, _T(""));
}
