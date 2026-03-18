
#include "pch.h"
#include "framework.h"
#include "WorkbookService.h"
#include "CsvInputReader.h"
#include "ExcelInputReader.h"
#include "JsonInputReader.h"
#include "SageException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void WorkbookService::LoadFromFile(const CString& strFilePath, TabularData& outData)
{
    int nDot = strFilePath.ReverseFind(_T('.'));
    if (nDot < 0)
        throw SageException(_T("파일 확장자를 확인할 수 없습니다."), strFilePath);

    CString strExt = strFilePath.Mid(nDot + 1);
    strExt.MakeLower();

    if (strExt == _T("csv")) {
        CsvInputReader reader;
        reader.Read(strFilePath, outData);
        return;
    }

    if (strExt == _T("xlsx") || strExt == _T("xls")) {
        ExcelInputReader reader;
        reader.Read(strFilePath, outData);
        return;
    }

    if (strExt == _T("json")) {
        JsonInputReader reader;
        reader.Read(strFilePath, outData);
        return;
    }

    CString strMsg;
    strMsg.Format(_T("지원하지 않는 파일 형식입니다: .%s"), (LPCTSTR)strExt);
    throw SageException(strMsg, strFilePath);
}
