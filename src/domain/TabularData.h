
#pragma once

#include <vector>

class CDataSheet
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

class CTabularData
{
public:
    CString                  m_strFilePath;
    std::vector<CDataSheet>  m_arrSheets;

    int GetSheetCount() const {
        return (int)m_arrSheets.size();
    }

    CDataSheet& GetSheet(int nIndex) {
        return m_arrSheets[nIndex];
    }

    const CDataSheet& GetSheet(int nIndex) const {
        return m_arrSheets[nIndex];
    }

    void Clear() {
        m_strFilePath.Empty();
        m_arrSheets.clear();
    }
};
