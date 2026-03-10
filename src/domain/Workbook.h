
#pragma once

// CWorksheet: 단일 시트의 행/열 데이터를 보관
class CWorksheet
{
public:
    CString                              m_strName;
    CArray<CStringArray*, CStringArray*> m_arrRows;

    CWorksheet() {}

    ~CWorksheet()
    {
        for (int i = 0; i < m_arrRows.GetSize(); i++)
            delete m_arrRows[i];
        m_arrRows.RemoveAll();
    }

    int GetRowCount() const
    {
        return (int)m_arrRows.GetSize();
    }

    int GetColumnCount() const
    {
        if (m_arrRows.IsEmpty())
            return 0;
        return (int)m_arrRows[0]->GetSize();
    }
};

// CWorkbook: 파일 하나를 표현하는 최상위 데이터 모델
class CWorkbook
{
public:
    CString                          m_strFilePath;
    CArray<CWorksheet*, CWorksheet*> m_arrSheets;

    CWorkbook() {}

    ~CWorkbook()
    {
        for (int i = 0; i < m_arrSheets.GetSize(); i++)
            delete m_arrSheets[i];
        m_arrSheets.RemoveAll();
    }

    int GetSheetCount() const
    {
        return (int)m_arrSheets.GetSize();
    }

    CWorksheet* GetSheet(int nIndex) const
    {
        return m_arrSheets[nIndex];
    }
};
