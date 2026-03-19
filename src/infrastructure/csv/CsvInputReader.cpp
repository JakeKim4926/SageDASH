
#include "pch.h"
#include "framework.h"
#include "CsvInputReader.h"
#include "SageException.h"
#include "Define.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CsvInputReader::Read(const CString& strFilePath, TabularData& outData)
{
    CStdioFile file;
    if (!file.Open(strFilePath, CFile::modeRead | CFile::typeText | CFile::shareDenyWrite))
        throw SageException(_T("파일을 열 수 없습니다."), strFilePath);

    outData.m_strFilePath = strFilePath;
    outData.m_arrSheets.emplace_back();
    DataSheet& sheet = outData.m_arrSheets.back();
    sheet.m_strName = _T("Sheet1");

    CString strLine;
    BOOL isFirstLine = TRUE;
    TCHAR chDelim = CSV_DELIM_COMMA;

    while (file.ReadString(strLine)) {
        if (isFirstLine) {
            if (strLine.GetLength() >= 3 &&
                strLine[0] == _T('\xEF') &&
                strLine[1] == _T('\xBB') &&
                strLine[2] == _T('\xBF'))
                strLine = strLine.Mid(3);

            chDelim = DetectDelimiter(strLine);
            isFirstLine = FALSE;
        }
        sheet.m_arrRows.push_back(ParseLine(strLine, chDelim));
    }

    file.Close();

    if (sheet.GetRowCount() == 0)
        throw SageException(_T("파일이 비어 있습니다."), strFilePath);
}

TCHAR CsvInputReader::DetectDelimiter(const CString& strFirstLine)
{
    int nComma     = 0;
    int nTab       = 0;
    int nSemicolon = 0;
    int nLen       = strFirstLine.GetLength();

    for (int i = 0; i < nLen; i++) {
        TCHAR ch = strFirstLine[i];
        if (ch == CSV_DELIM_COMMA)     nComma++;
        else if (ch == CSV_DELIM_TAB)  nTab++;
        else if (ch == CSV_DELIM_SEMICOLON) nSemicolon++;
    }

    if (nTab >= nComma && nTab >= nSemicolon)
        return CSV_DELIM_TAB;
    if (nSemicolon >= nComma)
        return CSV_DELIM_SEMICOLON;
    return CSV_DELIM_COMMA;
}

std::vector<CString> CsvInputReader::ParseLine(const CString& strLine, TCHAR chDelim)
{
    std::vector<CString> row;
    CString strCell;
    BOOL isInQuote = FALSE;
    int nLen = strLine.GetLength();

    for (int i = 0; i < nLen; i++) {
        TCHAR ch = strLine[i];

        if (isInQuote) {
            if (ch == _T('"')) {
                if (i + 1 < nLen && strLine[i + 1] == _T('"')) {
                    strCell += _T('"');
                    i++;
                } else {
                    isInQuote = FALSE;
                }
            } else {
                strCell += ch;
            }
        } else {
            if (ch == _T('"')) {
                isInQuote = TRUE;
            } else if (ch == chDelim) {
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
