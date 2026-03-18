
#include "pch.h"
#include "framework.h"
#include "JsonInputReader.h"
#include "SageException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ============================================================
// JsonParser — 내부 파서 (파일 범위)
// ============================================================
namespace {

class JsonParser
{
public:
    JsonParser(const CString& str) : m_str(str), m_nPos(0) {}

    void Parse(DataSheet& sheet);

private:
    const CString& m_str;
    int            m_nPos;

    void   SkipWs();
    TCHAR  Peek();
    TCHAR  Consume();
    BOOL   Expect(TCHAR ch);

    CString ParseString();
    CString ParseValue();
    void    ParseObject(std::vector<CString>& keys, std::vector<CString>& vals);
    void    ParseArrayOfObjects(DataSheet& sheet);
    void    ParseArrayOfArrays(DataSheet& sheet);
};

// -------- 유틸리티 --------

void JsonParser::SkipWs()
{
    while (m_nPos < m_str.GetLength() && _istspace(m_str[m_nPos]))
        m_nPos++;
}

TCHAR JsonParser::Peek()
{
    SkipWs();
    return (m_nPos < m_str.GetLength()) ? m_str[m_nPos] : _T('\0');
}

TCHAR JsonParser::Consume()
{
    return (m_nPos < m_str.GetLength()) ? m_str[m_nPos++] : _T('\0');
}

BOOL JsonParser::Expect(TCHAR ch)
{
    SkipWs();
    if (m_nPos < m_str.GetLength() && m_str[m_nPos] == ch) {
        m_nPos++;
        return TRUE;
    }
    return FALSE;
}

// -------- 값 파싱 --------

CString JsonParser::ParseString()
{
    Expect(_T('"'));
    CString result;
    while (m_nPos < m_str.GetLength()) {
        TCHAR ch = m_str[m_nPos++];
        if (ch == _T('"'))
            break;
        if (ch == _T('\\') && m_nPos < m_str.GetLength()) {
            TCHAR esc = m_str[m_nPos++];
            switch (esc) {
            case _T('n'):  result += _T('\n'); break;
            case _T('r'):  result += _T('\r'); break;
            case _T('t'):  result += _T('\t'); break;
            default:       result += esc;      break;
            }
        } else {
            result += ch;
        }
    }
    return result;
}

CString JsonParser::ParseValue()
{
    SkipWs();
    if (m_nPos >= m_str.GetLength())
        return CString();

    if (m_str[m_nPos] == _T('"'))
        return ParseString();

    // 숫자/bool/null — 구분자까지 읽기
    CString val;
    while (m_nPos < m_str.GetLength()) {
        TCHAR c = m_str[m_nPos];
        if (c == _T(',') || c == _T('}') || c == _T(']'))
            break;
        val += c;
        m_nPos++;
    }
    val.Trim();
    if (val == _T("null"))
        return CString();
    return val;
}

// -------- 객체/배열 파싱 --------

void JsonParser::ParseObject(std::vector<CString>& keys, std::vector<CString>& vals)
{
    Expect(_T('{'));
    while (TRUE) {
        if (Peek() == _T('}')) { Consume(); break; }
        if (Peek() == _T(',')) { Consume(); continue; }
        if (Peek() == _T('\0')) break;

        CString key = ParseString();
        Expect(_T(':'));
        CString val = ParseValue();
        keys.push_back(key);
        vals.push_back(val);
    }
}

void JsonParser::ParseArrayOfObjects(DataSheet& sheet)
{
    Expect(_T('['));
    std::vector<CString> headers;
    BOOL bFirst = TRUE;

    while (TRUE) {
        if (Peek() == _T(']')) { Consume(); break; }
        if (Peek() == _T(',')) { Consume(); continue; }
        if (Peek() == _T('\0')) break;

        std::vector<CString> keys, vals;
        ParseObject(keys, vals);

        if (bFirst) {
            headers = keys;
            sheet.m_arrRows.push_back(headers);
            bFirst = FALSE;
        }

        // 헤더 순서에 맞게 행 구성
        std::vector<CString> row(headers.size());
        for (int i = 0; i < (int)keys.size(); i++) {
            for (int j = 0; j < (int)headers.size(); j++) {
                if (keys[i] == headers[j]) {
                    row[j] = vals[i];
                    break;
                }
            }
        }
        sheet.m_arrRows.push_back(row);
    }
}

void JsonParser::ParseArrayOfArrays(DataSheet& sheet)
{
    Expect(_T('['));
    while (TRUE) {
        if (Peek() == _T(']')) { Consume(); break; }
        if (Peek() == _T(',')) { Consume(); continue; }
        if (Peek() == _T('\0')) break;

        std::vector<CString> rowVals;
        Expect(_T('['));
        while (TRUE) {
            if (Peek() == _T(']')) { Consume(); break; }
            if (Peek() == _T(',')) { Consume(); continue; }
            if (Peek() == _T('\0')) break;
            rowVals.push_back(ParseValue());
        }
        sheet.m_arrRows.push_back(rowVals);
    }
}

// -------- 진입점 --------

void JsonParser::Parse(DataSheet& sheet)
{
    if (Peek() != _T('['))
        throw SageException(_T("JSON 최상위가 배열([])이어야 합니다."), CString());

    int nSaved = m_nPos;
    m_nPos++;       // '[' 소비
    TCHAR firstElem = Peek();
    m_nPos = nSaved;    // 되돌리기

    if (firstElem == _T('{'))
        ParseArrayOfObjects(sheet);
    else if (firstElem == _T('['))
        ParseArrayOfArrays(sheet);
    else
        throw SageException(
            _T("지원하지 않는 JSON 형식입니다. 객체 배열 또는 배열의 배열이어야 합니다."),
            CString());
}

} // namespace

// ============================================================
// JsonInputReader::Read
// ============================================================

void JsonInputReader::Read(const CString& strFilePath, TabularData& outData)
{
    // 파일 전체를 줄 단위로 읽어 하나의 문자열로 합침
    CStdioFile file;
    if (!file.Open(strFilePath, CFile::modeRead | CFile::typeText | CFile::shareDenyWrite))
        throw SageException(_T("파일을 열 수 없습니다."), strFilePath);

    CString strJson;
    CString strLine;
    BOOL bFirst = TRUE;
    while (file.ReadString(strLine)) {
        // UTF-8 BOM 제거
        if (bFirst) {
            if (strLine.GetLength() >= 3 &&
                (unsigned char)strLine[0] == 0xEF &&
                (unsigned char)strLine[1] == 0xBB &&
                (unsigned char)strLine[2] == 0xBF)
                strLine = strLine.Mid(3);
            bFirst = FALSE;
        }
        strJson += strLine;
        strJson += _T('\n');
    }
    file.Close();

    if (strJson.IsEmpty())
        throw SageException(_T("파일이 비어 있습니다."), strFilePath);

    outData.m_strFilePath = strFilePath;
    outData.m_arrSheets.emplace_back();
    DataSheet& sheet = outData.m_arrSheets.back();
    sheet.m_strName = _T("Sheet1");

    JsonParser parser(strJson);
    parser.Parse(sheet);
}
