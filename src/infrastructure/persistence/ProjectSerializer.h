
#pragma once

#include "AutomationProject.h"

// ============================================================
// ProjectSerializer — AutomationProject ↔ .sagep 파일 (INI 형식)
// ============================================================
class ProjectSerializer
{
public:
    void Save(const AutomationProject& project, const CString& strFilePath);
    void Load(const CString& strFilePath, AutomationProject& outProject);

private:
    static CString EscapeValue(const CString& str);
    static CString UnescapeValue(const CString& str);
};
