
#include "pch.h"
#include "framework.h"
#include "CsvReader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWorksheet* CCsvReader::ReadFile(const CString& strFilePath, CString& strError)
{
    CStdioFile file;
    if (!file.Open(strFilePath, CFile::modeRead | CFile::typeText | CFile::shareDenyWrite)) {
        strError.Format(_T("파일을 열 수 없습니다: %s"), (LPCTSTR)strFilePath);
        return nullptr;
    }

    CWorksheet* pSheet = new CWorksheet();
    pSheet->m_strName = _T("Sheet1");

    CString strLine;
    while (file.ReadString(strLine)) {
        pSheet->m_arrRows.push_back(ParseLine(strLine));
    }

    file.Close();

    if (pSheet->GetRowCount() == 0) {
        strError = _T("파일이 비어 있습니다.");
        delete pSheet;
        return nullptr;
    }

    return pSheet;
}

std::vector<CString> CCsvReader::ParseLine(const CString& strLine)
{
    std::vector<CString> row;
    CString strCell;
    bool isInQuote = false;
    int nLen = strLine.GetLength();

    for (int i = 0; i < nLen; i++) {
        TCHAR ch = strLine[i];

        if (isInQuote) {
            if (ch == _T('"')) {
                // "" → 이스케이프된 따옴표
                if (i + 1 < nLen && strLine[i + 1] == _T('"')) {
                    strCell += _T('"');
                    i++;
                } else {
                    isInQuote = false;
                }
            } else {
                strCell += ch;
            }
        } else {
            if (ch == _T('"')) {
                isInQuote = true;
            } else if (ch == _T(',')) {
                row.push_back(strCell);
                strCell.Empty();
            } else {
                strCell += ch;
            }
        }
    }

    row.push_back(strCell);
    return row;
}
