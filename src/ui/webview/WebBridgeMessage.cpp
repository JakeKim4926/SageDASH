#include "pch.h"
#include "WebBridgeMessage.h"
#include "ValidationResult.h"
#include "TabularData.h"

CString WebBridgeMessage::BuildNativeReady()
{
    return Build(BRIDGE_TYPE_NATIVE_READY, _T("{}"));
}

CString WebBridgeMessage::BuildDataSummary(int nRowCount, int nColCount, const CString& strFileName)
{
    CString strPayload;
    strPayload.Format(
        _T("{\"rowCount\":%d,\"colCount\":%d,\"fileName\":\"%s\"}"),
        nRowCount, nColCount, (LPCTSTR)EscapeString(strFileName));
    return Build(BRIDGE_TYPE_DATA_SUMMARY, strPayload);
}

CString WebBridgeMessage::BuildValidationReport(const ValidationResult& result)
{
    int nErrors   = result.GetErrorCount();
    int nWarnings = result.GetWarningCount();
    int nTotal    = result.GetIssueCount();

    // 최대 50건만 직렬화
    constexpr int MAX_ISSUES = 50;
    CString strIssues = _T("[");
    int nLimit = (nTotal < MAX_ISSUES) ? nTotal : MAX_ISSUES;
    for (int i = 0; i < nLimit; i++) {
        const ValidationIssue& issue = result.GetIssue(i);
        CString strSeverity = issue.IsError() ? _T("error") : _T("warning");
        CString strItem;
        strItem.Format(
            _T("{\"row\":%d,\"column\":\"%s\",\"message\":\"%s\",\"severity\":\"%s\"}"),
            issue.GetRow(),
            (LPCTSTR)EscapeString(issue.GetColumn()),
            (LPCTSTR)EscapeString(issue.GetMessage()),
            (LPCTSTR)strSeverity);
        if (i > 0) strIssues += _T(",");
        strIssues += strItem;
    }
    strIssues += _T("]");

    CString strPayload;
    strPayload.Format(
        _T("{\"errorCount\":%d,\"warningCount\":%d,\"totalIssues\":%d,\"truncated\":%s,\"issues\":%s}"),
        nErrors, nWarnings, nTotal,
        (nTotal > MAX_ISSUES ? _T("true") : _T("false")),
        (LPCTSTR)strIssues);
    return Build(BRIDGE_TYPE_VALIDATION_REPORT, strPayload);
}

CString WebBridgeMessage::BuildColumnAnalysis(const DataSheet& sheet)
{
    CString strColumns = _T("[");

    int nRowCount = sheet.GetRowCount();
    if (nRowCount < 1) {
        return Build(BRIDGE_TYPE_COLUMN_ANALYSIS, _T("{\"columns\":[]}"));
    }

    const std::vector<CString>& arrHeaders = sheet.m_arrRows[0];
    int nColCount = (int)arrHeaders.size();

    for (int col = 0; col < nColCount; col++) {
        int nFilled  = 0;
        int nEmpty   = 0;
        // 데이터 행 순회 (0번은 헤더)
        for (int row = 1; row < nRowCount; row++) {
            const std::vector<CString>& arrRow = sheet.m_arrRows[row];
            if (col < (int)arrRow.size() && !arrRow[col].IsEmpty())
                nFilled++;
            else
                nEmpty++;
        }

        CString strItem;
        strItem.Format(
            _T("{\"name\":\"%s\",\"filledCount\":%d,\"emptyCount\":%d}"),
            (LPCTSTR)EscapeString(arrHeaders[col]),
            nFilled, nEmpty);
        if (col > 0) strColumns += _T(",");
        strColumns += strItem;
    }
    strColumns += _T("]");

    CString strPayload;
    strPayload.Format(
        _T("{\"rowCount\":%d,\"colCount\":%d,\"columns\":%s}"),
        nRowCount > 0 ? nRowCount - 1 : 0,
        nColCount,
        (LPCTSTR)strColumns);
    return Build(BRIDGE_TYPE_COLUMN_ANALYSIS, strPayload);
}

CString WebBridgeMessage::Build(const CString& strType, const CString& strPayloadJson)
{
    CString strMsg;
    strMsg.Format(
        _T("{\"type\":\"%s\",\"version\":\"1.0\",\"payload\":%s}"),
        (LPCTSTR)strType, (LPCTSTR)strPayloadJson);
    return strMsg;
}

CString WebBridgeMessage::ParseType(const CString& strJson)
{
    const CString strKey = _T("\"type\":\"");
    int nStart = strJson.Find(strKey);
    if (nStart < 0) {
        return _T("");
    }
    nStart += strKey.GetLength();
    int nEnd = strJson.Find(_T("\""), nStart);
    if (nEnd < 0) {
        return _T("");
    }
    return strJson.Mid(nStart, nEnd - nStart);
}

CString WebBridgeMessage::EscapeString(const CString& strValue)
{
    CString strResult = strValue;
    strResult.Replace(_T("\\"), _T("\\\\"));
    strResult.Replace(_T("\""), _T("\\\""));
    strResult.Replace(_T("\r"), _T("\\r"));
    strResult.Replace(_T("\n"), _T("\\n"));
    strResult.Replace(_T("\t"), _T("\\t"));
    return strResult;
}
