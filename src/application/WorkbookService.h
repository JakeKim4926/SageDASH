
#pragma once

#include "TabularData.h"

class WorkbookService
{
public:
    void LoadFromFile(const CString& strFilePath, TabularData& outData);
    void LoadFromFolder(const CString& strFolderPath, TabularData& outData);
};
