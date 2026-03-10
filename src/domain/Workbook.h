
#pragma once

#include <vector>

// CWorksheet: 단일 시트의 행/열 데이터를 보관
class CWorksheet
{
public:
    CString                             m_strName;
    std::vector<std::vector<CString>>   m_arrRows;

    int GetRowCount() const {
        return (int)m_arrRows.size();
    }

    int GetColumnCount() const {
        if (m_arrRows.empty())
            return 0;
        return (int)m_arrRows[0].size();
    }
};

// CWorkbook: 파일 하나를 표현하는 최상위 데이터 모델
class CWorkbook
{
public:
    CString                  m_strFilePath;
    std::vector<CWorksheet*> m_arrSheets;

    CWorkbook() {}

    ~CWorkbook() {
        for (int i = 0; i < (int)m_arrSheets.size(); i++)
            delete m_arrSheets[i];
        m_arrSheets.clear();
    }

    int GetSheetCount() const {
        return (int)m_arrSheets.size();
    }

    CWorksheet* GetSheet(int nIndex) const {
        return m_arrSheets[nIndex];
    }
};
