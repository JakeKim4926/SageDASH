
#include "pch.h"
#include "framework.h"
#include "ProjectService.h"
#include "../infrastructure/persistence/ProjectSerializer.h"
#include "SageException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString ProjectService::SaveProject(const AutomationProject& project,
                                    const CString& strFilePath) const
{
    try {
        ProjectSerializer serializer;
        serializer.Save(project, strFilePath);
    } catch (const SageException& e) {
        return e.Format();
    } catch (...) {
        return _T("알 수 없는 오류가 발생했습니다.");
    }
    return CString();
}

CString ProjectService::LoadProject(const CString& strFilePath,
                                    AutomationProject& outProject) const
{
    try {
        ProjectSerializer serializer;
        serializer.Load(strFilePath, outProject);
    } catch (const SageException& e) {
        return e.Format();
    } catch (...) {
        return _T("알 수 없는 오류가 발생했습니다.");
    }
    return CString();
}
