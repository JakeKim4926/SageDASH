
#include "pch.h"
#include "framework.h"
#include "ExportService.h"
#include "FileOutputTarget.h"
#include "CsvOutputWriter.h"
#include "ExcelOutputWriter.h"
#include "SageException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString ExportService::Export(const TabularData& data, const CString& strFilePath) const
{
    FileOutputTarget target(strFilePath);

    CString strExt = strFilePath;
    int nDot = strExt.ReverseFind(_T('.'));
    if (nDot >= 0)
        strExt = strExt.Mid(nDot + 1);
    strExt.MakeLower();

    try {
        if (strExt == _T("xlsx") || strExt == _T("xls")) {
            ExcelOutputWriter writer;
            writer.Write(data, target);
        } else {
            CsvOutputWriter writer;
            writer.Write(data, target);
        }
    } catch (const SageException& e) {
        return e.GetMessage();
    } catch (...) {
        return _T("알 수 없는 오류가 발생했습니다.");
    }

    return CString();
}
