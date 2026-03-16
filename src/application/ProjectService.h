
#pragma once

#include "AutomationProject.h"

// ============================================================
// ProjectService — 프로젝트 저장/불러오기 (Phase 3)
// ============================================================
class ProjectService
{
public:
    // 저장 성공 시 빈 CString, 실패 시 오류 메시지 반환
    CString SaveProject(const AutomationProject& project, const CString& strFilePath) const;

    // 로드 성공 시 빈 CString, 실패 시 오류 메시지 반환
    CString LoadProject(const CString& strFilePath, AutomationProject& outProject) const;
};
