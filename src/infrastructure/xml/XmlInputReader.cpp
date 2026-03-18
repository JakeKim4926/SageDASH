
#include "pch.h"
#include "framework.h"
#include "XmlInputReader.h"
#include "SageException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ============================================================
// XmlParser — 내부 파서 (파일 범위)
// ============================================================
namespace {

// -------- 문자열 유틸리티 --------

static CString XmlDecode(const CString& str)
{
    CString result = str;
    result.Replace(_T("&amp;"),  _T("&"));
    result.Replace(_T("&lt;"),   _T("<"));
    result.Replace(_T("&gt;"),   _T(">"));
    result.Replace(_T("&quot;"), _T("\""));
    result.Replace(_T("&apos;"), _T("'"));
    return result;
}

// -------- 토큰 --------

enum XmlTokenType
{
    XML_TOK_OPEN_TAG,       // <tagName ...>  또는  <tagName ... />
    XML_TOK_CLOSE_TAG,      // </tagName>
    XML_TOK_TEXT,           // 텍스트 노드
    XML_TOK_EOF,
};

struct XmlToken
{
    XmlTokenType             eType;
    CString                  strName;       // 엘리먼트 이름
    std::vector<CString>     arrAttrKeys;
    std::vector<CString>     arrAttrVals;
    BOOL                     bSelfClose;    // />
    CString                  strText;       // TEXT 토큰 내용
};

// -------- 렉서 --------

class XmlLexer
{
public:
    XmlLexer(const CString& str) : m_str(str), m_nPos(0) {}

    XmlToken Next();

private:
    const CString& m_str;
    int            m_nPos;

    void    SkipWs();
    CString ReadName();
    CString ReadAttrValue();
    void    ParseAttributes(XmlToken& tok);
};

void XmlLexer::SkipWs()
{
    while (m_nPos < m_str.GetLength() && _istspace(m_str[m_nPos]))
        m_nPos++;
}

CString XmlLexer::ReadName()
{
    CString name;
    while (m_nPos < m_str.GetLength()) {
        TCHAR c = m_str[m_nPos];
        if (_istspace(c) || c == _T('>') || c == _T('/') || c == _T('='))
            break;
        name += c;
        m_nPos++;
    }
    return name;
}

CString XmlLexer::ReadAttrValue()
{
    SkipWs();
    if (m_nPos >= m_str.GetLength())
        return CString();

    TCHAR quote = m_str[m_nPos];
    if (quote != _T('"') && quote != _T('\''))
        return CString();
    m_nPos++;

    CString val;
    while (m_nPos < m_str.GetLength()) {
        TCHAR c = m_str[m_nPos++];
        if (c == quote)
            break;
        val += c;
    }
    return XmlDecode(val);
}

void XmlLexer::ParseAttributes(XmlToken& tok)
{
    while (TRUE) {
        SkipWs();
        if (m_nPos >= m_str.GetLength())
            break;
        TCHAR c = m_str[m_nPos];
        if (c == _T('>') || c == _T('/'))
            break;

        CString key = ReadName();
        if (key.IsEmpty())
            break;

        SkipWs();
        if (m_nPos < m_str.GetLength() && m_str[m_nPos] == _T('=')) {
            m_nPos++;
            CString val = ReadAttrValue();
            tok.arrAttrKeys.push_back(key);
            tok.arrAttrVals.push_back(val);
        } else {
            // 값 없는 속성 — 빈 값으로 처리
            tok.arrAttrKeys.push_back(key);
            tok.arrAttrVals.push_back(CString());
        }
    }
}

XmlToken XmlLexer::Next()
{
    XmlToken tok;
    tok.bSelfClose = FALSE;

    if (m_nPos >= m_str.GetLength()) {
        tok.eType = XML_TOK_EOF;
        return tok;
    }

    // XML 선언/처리 명령/주석/CDATA 건너뛰기
    while (m_nPos < m_str.GetLength()) {
        SkipWs();
        if (m_nPos >= m_str.GetLength())
            break;

        if (m_str[m_nPos] != _T('<'))
            break;

        // 다음 문자 미리 보기
        if (m_nPos + 1 >= m_str.GetLength())
            break;

        TCHAR ch2 = m_str[m_nPos + 1];

        if (ch2 == _T('?')) {
            // <?...?> 건너뛰기
            m_nPos += 2;
            while (m_nPos + 1 < m_str.GetLength()) {
                if (m_str[m_nPos] == _T('?') && m_str[m_nPos + 1] == _T('>')) {
                    m_nPos += 2;
                    break;
                }
                m_nPos++;
            }
            continue;
        }

        if (ch2 == _T('!')) {
            if (m_nPos + 3 < m_str.GetLength() &&
                m_str[m_nPos + 2] == _T('-') && m_str[m_nPos + 3] == _T('-')) {
                // <!-- ... --> 주석 건너뛰기
                m_nPos += 4;
                while (m_nPos + 2 < m_str.GetLength()) {
                    if (m_str[m_nPos]     == _T('-') &&
                        m_str[m_nPos + 1] == _T('-') &&
                        m_str[m_nPos + 2] == _T('>')) {
                        m_nPos += 3;
                        break;
                    }
                    m_nPos++;
                }
                continue;
            }
            if (m_nPos + 8 < m_str.GetLength() &&
                m_str.Mid(m_nPos + 2, 7) == _T("[CDATA[")) {
                // <![CDATA[...]]> 건너뛰기 (CDATA 내용은 무시)
                m_nPos += 9;
                while (m_nPos + 2 < m_str.GetLength()) {
                    if (m_str[m_nPos]     == _T(']') &&
                        m_str[m_nPos + 1] == _T(']') &&
                        m_str[m_nPos + 2] == _T('>')) {
                        m_nPos += 3;
                        break;
                    }
                    m_nPos++;
                }
                continue;
            }
            // <!DOCTYPE 등 — '>' 까지 건너뛰기
            while (m_nPos < m_str.GetLength() && m_str[m_nPos] != _T('>'))
                m_nPos++;
            if (m_nPos < m_str.GetLength())
                m_nPos++;
            continue;
        }

        break;
    }

    SkipWs();
    if (m_nPos >= m_str.GetLength()) {
        tok.eType = XML_TOK_EOF;
        return tok;
    }

    if (m_str[m_nPos] != _T('<')) {
        // 텍스트 노드
        tok.eType = XML_TOK_TEXT;
        while (m_nPos < m_str.GetLength() && m_str[m_nPos] != _T('<')) {
            tok.strText += m_str[m_nPos++];
        }
        tok.strText.Trim();
        tok.strText = XmlDecode(tok.strText);
        return tok;
    }

    m_nPos++;   // '<' 소비

    if (m_nPos < m_str.GetLength() && m_str[m_nPos] == _T('/')) {
        // 닫는 태그
        m_nPos++;
        tok.eType = XML_TOK_CLOSE_TAG;
        tok.strName = ReadName();
        // '>' 까지 건너뛰기
        while (m_nPos < m_str.GetLength() && m_str[m_nPos] != _T('>'))
            m_nPos++;
        if (m_nPos < m_str.GetLength())
            m_nPos++;
        return tok;
    }

    // 여는 태그
    tok.eType = XML_TOK_OPEN_TAG;
    tok.strName = ReadName();
    ParseAttributes(tok);

    SkipWs();
    if (m_nPos < m_str.GetLength() && m_str[m_nPos] == _T('/')) {
        tok.bSelfClose = TRUE;
        m_nPos++;
    }
    if (m_nPos < m_str.GetLength() && m_str[m_nPos] == _T('>'))
        m_nPos++;

    return tok;
}

// -------- 파서 --------

class XmlParser
{
public:
    XmlParser(const CString& str) : m_lexer(str) {}

    void Parse(DataSheet& sheet);

private:
    XmlLexer m_lexer;

    // 루트 자식 행 파싱
    void ParseRow(std::vector<CString>& keys, std::vector<CString>& vals);
};

void XmlParser::ParseRow(std::vector<CString>& keys, std::vector<CString>& vals)
{
    // 호출 시점에 이미 <row ...> 토큰은 소비된 상태
    // 자식 엘리먼트를 반복해서 읽는다
    while (TRUE) {
        XmlToken tok = m_lexer.Next();
        if (tok.eType == XML_TOK_EOF)
            break;
        if (tok.eType == XML_TOK_CLOSE_TAG)
            break;
        if (tok.eType == XML_TOK_TEXT)
            continue;
        if (tok.eType != XML_TOK_OPEN_TAG)
            continue;

        CString strChildName = tok.strName;

        if (tok.bSelfClose) {
            // <col />
            keys.push_back(strChildName);
            vals.push_back(CString());
            continue;
        }

        // 텍스트 내용 읽기
        CString strText;
        while (TRUE) {
            XmlToken inner = m_lexer.Next();
            if (inner.eType == XML_TOK_EOF)
                break;
            if (inner.eType == XML_TOK_CLOSE_TAG)
                break;
            if (inner.eType == XML_TOK_TEXT)
                strText += inner.strText;
            // 중첩 엘리먼트는 무시 (평탄화)
        }
        strText.Trim();
        keys.push_back(strChildName);
        vals.push_back(strText);
    }
}

void XmlParser::Parse(DataSheet& sheet)
{
    // 루트 엘리먼트 탐색
    while (TRUE) {
        XmlToken tok = m_lexer.Next();
        if (tok.eType == XML_TOK_EOF)
            throw SageException(_T("XML 루트 엘리먼트를 찾을 수 없습니다."), CString());
        if (tok.eType == XML_TOK_OPEN_TAG) {
            if (tok.bSelfClose)
                throw SageException(_T("XML 루트 엘리먼트가 비어 있습니다."), CString());
            break;
        }
    }

    std::vector<CString> headers;
    BOOL bFirst = TRUE;

    while (TRUE) {
        XmlToken tok = m_lexer.Next();
        if (tok.eType == XML_TOK_EOF)
            break;
        if (tok.eType == XML_TOK_CLOSE_TAG)
            break;  // 루트 닫힘
        if (tok.eType == XML_TOK_TEXT)
            continue;
        if (tok.eType != XML_TOK_OPEN_TAG)
            continue;

        // 행 엘리먼트 처리
        std::vector<CString> keys;
        std::vector<CString> vals;

        // 속성을 먼저 읽기
        for (int i = 0; i < (int)tok.arrAttrKeys.size(); i++) {
            keys.push_back(tok.arrAttrKeys[i]);
            vals.push_back(tok.arrAttrVals[i]);
        }

        if (!tok.bSelfClose) {
            // 자식 엘리먼트에서 추가 컬럼 읽기
            ParseRow(keys, vals);
        }

        if (keys.empty())
            continue;

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

    if (sheet.m_arrRows.empty())
        throw SageException(_T("XML에서 읽을 수 있는 행이 없습니다."), CString());
}

} // namespace

// ============================================================
// XmlInputReader::Read
// ============================================================

void XmlInputReader::Read(const CString& strFilePath, TabularData& outData)
{
    CStdioFile file;
    if (!file.Open(strFilePath, CFile::modeRead | CFile::typeText | CFile::shareDenyWrite))
        throw SageException(_T("파일을 열 수 없습니다."), strFilePath);

    CString strXml;
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
        strXml += strLine;
        strXml += _T('\n');
    }
    file.Close();

    if (strXml.IsEmpty())
        throw SageException(_T("파일이 비어 있습니다."), strFilePath);

    outData.m_strFilePath = strFilePath;
    outData.m_arrSheets.emplace_back();
    DataSheet& sheet = outData.m_arrSheets.back();
    sheet.m_strName = _T("Sheet1");

    XmlParser parser(strXml);
    parser.Parse(sheet);
}
