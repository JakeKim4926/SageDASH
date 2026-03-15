
#pragma once

#include "TabularData.h"

class WorkbookService
{
public:
    void LoadFromFile(const CString& strFilePath, TabularData& outData);
};
