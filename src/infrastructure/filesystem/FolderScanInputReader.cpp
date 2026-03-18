
#include "pch.h"
#include "framework.h"
#include "FolderScanInputReader.h"
#include "SageException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ============================================================
// 폴더 스캔 — 헤더 컬럼 인덱스
// ============================================================
constexpr int COL_FILENAME  = 0;
constexpr int COL_EXT       = 1;
constexpr int COL_SIZE      = 2;
constexpr int COL_MODIFIED  = 3;
constexpr int COL_CREATED   = 4;
constexpr int COL_FULLPATH  = 5;
constexpr int COL_COUNT     = 6;

void FolderScanInputReader::Read(const CString& strFolderPath, TabularData& outData)
{
    outData.Clear();

    DWORD dwAttr = GetFileAttributes(strFolderPath);
    if (dwAttr == INVALID_FILE_ATTRIBUTES || !(dwAttr & FILE_ATTRIBUTE_DIRECTORY)) {
        throw SageException(_T("유효한 폴더 경로가 아닙니다."), strFolderPath);
    }

    DataSheet sheet;
    sheet.m_strName = _T("FolderScan");

    ScanFolder(strFolderPath, sheet);

    outData.m_strFilePath = strFolderPath;
    outData.m_arrSheets.push_back(sheet);
}

void FolderScanInputReader::ScanFolder(const CString& strFolderPath, DataSheet& outSheet)
{
    // 헤더 행 추가
    std::vector<CString> arrHeader(COL_COUNT);
    arrHeader[COL_FILENAME] = _T("파일명");
    arrHeader[COL_EXT]      = _T("확장자");
    arrHeader[COL_SIZE]     = _T("크기(bytes)");
    arrHeader[COL_MODIFIED] = _T("수정날짜");
    arrHeader[COL_CREATED]  = _T("생성날짜");
    arrHeader[COL_FULLPATH] = _T("전체경로");
    outSheet.m_arrRows.push_back(arrHeader);

    CString strPattern = strFolderPath;
    if (strPattern.Right(1) != _T("\\"))
        strPattern += _T("\\");
    strPattern += _T("*");

    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile(strPattern, &wfd);
    if (hFind == INVALID_HANDLE_VALUE)
        return;

    CString strBase = strFolderPath;
    if (strBase.Right(1) != _T("\\"))
        strBase += _T("\\");

    do {
        CString strName = wfd.cFileName;
        if (strName == _T(".") || strName == _T(".."))
            continue;
        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        CString strFullPath = strBase + strName;

        // 확장자 추출
        CString strExt;
        int nDot = strName.ReverseFind(_T('.'));
        if (nDot >= 0)
            strExt = strName.Mid(nDot + 1);

        // 파일 크기
        ULONGLONG ullSize = ((ULONGLONG)wfd.nFileSizeHigh << 32) | wfd.nFileSizeLow;
        CString strSize = FormatFileSize(ullSize);

        // 날짜
        CString strModified = FormatFileTime(wfd.ftLastWriteTime);
        CString strCreated  = FormatFileTime(wfd.ftCreationTime);

        std::vector<CString> arrRow(COL_COUNT);
        arrRow[COL_FILENAME] = strName;
        arrRow[COL_EXT]      = strExt;
        arrRow[COL_SIZE]     = strSize;
        arrRow[COL_MODIFIED] = strModified;
        arrRow[COL_CREATED]  = strCreated;
        arrRow[COL_FULLPATH] = strFullPath;
        outSheet.m_arrRows.push_back(arrRow);

    } while (FindNextFile(hFind, &wfd));

    FindClose(hFind);
}

CString FolderScanInputReader::FormatFileSize(ULONGLONG ullBytes)
{
    CString strResult;
    strResult.Format(_T("%llu"), ullBytes);
    return strResult;
}

CString FolderScanInputReader::FormatFileTime(const FILETIME& ft)
{
    SYSTEMTIME stUTC, stLocal;
    FileTimeToSystemTime(&ft, &stUTC);
    SystemTimeToTzSpecificLocalTime(nullptr, &stUTC, &stLocal);

    CString strResult;
    strResult.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
        stLocal.wYear, stLocal.wMonth, stLocal.wDay,
        stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
    return strResult;
}
