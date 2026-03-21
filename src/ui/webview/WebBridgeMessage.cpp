#include "pch.h"
#include "WebBridgeMessage.h"

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
