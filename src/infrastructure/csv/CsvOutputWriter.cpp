
#include "pch.h"
#include "framework.h"
#include "CsvOutputWriter.h"
#include "FileOutputTarget.h"
#include "SageException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CsvOutputWriter::Write(const TabularData& data, const IOutputTarget& target)
{
    const FileOutputTarget* pFileTarget =
        dynamic_cast<const FileOutputTarget*>(&target);
    if (pFileTarget == nullptr)
        throw SageException(_T("CsvOutputWriter: 잘못된 출력 대상입니다."));

    if (data.GetSheetCount() == 0 || data.GetSheet(0).GetRowCount() == 0)
        throw SageException(_T("내보낼 데이터가 없습니다."));

    const DataSheet& sheet = data.GetSheet(0);
    const CString& strPath = pFileTarget->GetFilePath();

    CStdioFile file;
    if (!file.Open(strPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
        CString msg;
        msg.Format(_T("파일을 열 수 없습니다: %s"), (LPCTSTR)strPath);
        throw SageException(msg);
    }

    // UTF-8 BOM
    file.Write("\xEF\xBB\xBF", 3);

    int nRows = sheet.GetRowCount();
    for (int r = 0; r < nRows; r++) {
        const std::vector<CString>& row = sheet.m_arrRows[r];
        CString strLine;
        for (int c = 0; c < (int)row.size(); c++) {
            if (c > 0)
                strLine += _T(",");
            strLine += EscapeCell(row[c]);
        }
        strLine += _T("\r\n");
        file.WriteString(strLine);
    }

    file.Close();
}

CString CsvOutputWriter::EscapeCell(const CString& str)
{
    BOOL bNeedQuote = (str.Find(_T(',')) >= 0 ||
                       str.Find(_T('"')) >= 0 ||
                       str.Find(_T('\n')) >= 0 ||
                       str.Find(_T('\r')) >= 0);
    if (!bNeedQuote)
        return str;

    CString escaped = str;
    escaped.Replace(_T("\""), _T("\"\""));
    return _T("\"") + escaped + _T("\"");
}
