
#include "pch.h"
#include "framework.h"
#include "CsvReader.h"
#include "SageException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CCsvReader::ReadFile(const CString& strFilePath, CWorksheet& outSheet)
{
    CStdioFile file;
    if (!file.Open(strFilePath, CFile::modeRead | CFile::typeText | CFile::shareDenyWrite))
        throw SageException(_T("파일을 열 수 없습니다."), strFilePath);

    outSheet.m_strName = _T("Sheet1");
    outSheet.m_arrRows.clear();

    CString strLine;
    bool isFirstLine = true;
    while (file.ReadString(strLine)) {
        // UTF-8 BOM (EF BB BF) 제거 — CStdioFile이 바이트 단위로 읽어 3개 문자로 나타남
        if (isFirstLine) {
            if (strLine.GetLength() >= 3 &&
                strLine[0] == _T('\xEF') &&
                strLine[1] == _T('\xBB') &&
                strLine[2] == _T('\xBF'))
                strLine = strLine.Mid(3);
            isFirstLine = false;
        }
        outSheet.m_arrRows.push_back(ParseLine(strLine));
    }

    file.Close();

    if (outSheet.GetRowCount() == 0)
        throw SageException(_T("파일이 비어 있습니다."), strFilePath);
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
