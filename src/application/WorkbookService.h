
#pragma once

#include "TabularData.h"

// 실패 시 SageException을 던진다
class WorkbookService
{
public:
    void LoadFromFile(const CString& strFilePath, CTabularData& outData);

private:
    std::unique_ptr<class IInputReader> CreateReader(const CString& strExt);
};
