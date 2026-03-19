
#include "pch.h"
#include "framework.h"
#include "DbInputReader.h"
#include "SageException.h"
#include <sql.h>
#include <sqlext.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ============================================================
// .dbq 파일 형식
//   1행: ODBC 연결 문자열
//   2행~: SQL 쿼리 (멀티라인 가능)
// ============================================================

constexpr int DB_MAX_COL_NAME  = 256;
constexpr int DB_MAX_CELL_DATA = 4096;

void DbInputReader::Read(const CString& strDbqPath, TabularData& outData)
{
    outData.Clear();

    CString strConnStr;
    CString strQuery;
    if (!ParseDbqFile(strDbqPath, strConnStr, strQuery)) {
        throw SageException(_T(".dbq 파일 형식이 올바르지 않습니다.\n1행: 연결 문자열, 2행~: SQL 쿼리"), strDbqPath);
    }

    ExecuteQuery(strConnStr, strQuery, outData);
    outData.m_strFilePath = strDbqPath;
}

BOOL DbInputReader::ParseDbqFile(const CString& strPath,
                                  CString& strConnStr,
                                  CString& strQuery)
{
    CStdioFile file;
    if (!file.Open(strPath, CFile::modeRead | CFile::typeText))
        return FALSE;

    CString strLine;
    BOOL isFirstLine = TRUE;
    strConnStr.Empty();
    strQuery.Empty();

    while (file.ReadString(strLine)) {
        strLine.TrimRight(_T("\r\n"));
        if (isFirstLine) {
            strConnStr = strLine;
            isFirstLine = FALSE;
        } else {
            if (!strQuery.IsEmpty())
                strQuery += _T("\r\n");
            strQuery += strLine;
        }
    }

    file.Close();

    strConnStr.Trim();
    strQuery.Trim();

    return (!strConnStr.IsEmpty() && !strQuery.IsEmpty());
}

void DbInputReader::ExecuteQuery(const CString& strConnStr,
                                  const CString& strQuery,
                                  TabularData& outData)
{
    SQLHENV  hEnv  = SQL_NULL_HENV;
    SQLHDBC  hDbc  = SQL_NULL_HDBC;
    SQLHSTMT hStmt = SQL_NULL_HSTMT;
    SQLRETURN ret;

    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
        throw SageException(_T("ODBC 환경 핸들 생성에 실패했습니다."), strConnStr);

    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        throw SageException(_T("ODBC 연결 핸들 생성에 실패했습니다."), strConnStr);
    }

    ret = SQLDriverConnect(hDbc, NULL,
        (SQLWCHAR*)(LPCTSTR)strConnStr, SQL_NTS,
        NULL, 0, NULL, SQL_DRIVER_NOPROMPT);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLWCHAR szState[6] = {}, szMsg[512] = {};
        SQLINTEGER nErr = 0;
        SQLSMALLINT nMsgLen = 0;
        SQLGetDiagRec(SQL_HANDLE_DBC, hDbc, 1, szState, &nErr, szMsg, 512, &nMsgLen);
        CString strErr;
        strErr.Format(_T("DB 연결 실패: [%s] %s"), (LPCTSTR)CString(szState), (LPCTSTR)CString(szMsg));
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        throw SageException(strErr, strConnStr);
    }

    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        throw SageException(_T("ODBC 구문 핸들 생성에 실패했습니다."), strQuery);
    }

    ret = SQLExecDirect(hStmt, (SQLWCHAR*)(LPCTSTR)strQuery, SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLWCHAR szState[6] = {}, szMsg[512] = {};
        SQLINTEGER nErr = 0;
        SQLSMALLINT nMsgLen = 0;
        SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, szState, &nErr, szMsg, 512, &nMsgLen);
        CString strErr;
        strErr.Format(_T("쿼리 실행 실패: [%s] %s"), (LPCTSTR)CString(szState), (LPCTSTR)CString(szMsg));
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        throw SageException(strErr, strQuery);
    }

    SQLSMALLINT nColCount = 0;
    SQLNumResultCols(hStmt, &nColCount);

    DataSheet sheet;
    sheet.m_strName = _T("QueryResult");

    // 헤더 행 구성
    std::vector<CString> arrHeader;
    for (SQLSMALLINT i = 1; i <= nColCount; i++) {
        SQLWCHAR szColName[DB_MAX_COL_NAME] = {};
        SQLSMALLINT nColNameLen = 0;
        SQLDescribeCol(hStmt, i, szColName, DB_MAX_COL_NAME, &nColNameLen,
                       NULL, NULL, NULL, NULL);
        arrHeader.push_back(CString(szColName));
    }
    sheet.m_arrRows.push_back(arrHeader);

    // 데이터 행 수집
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        std::vector<CString> arrRow;
        for (SQLSMALLINT i = 1; i <= nColCount; i++) {
            SQLWCHAR szData[DB_MAX_CELL_DATA] = {};
            SQLLEN nIndicator = 0;
            ret = SQLGetData(hStmt, i, SQL_C_WCHAR, szData, sizeof(szData), &nIndicator);
            if (nIndicator == SQL_NULL_DATA)
                arrRow.push_back(_T(""));
            else
                arrRow.push_back(CString(szData));
        }
        sheet.m_arrRows.push_back(arrRow);
    }

    outData.m_arrSheets.push_back(sheet);

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}
