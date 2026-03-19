
#pragma once

#include "AutomationProject.h"

// ============================================================
// BatchJob — 배치 실행 단위 (Phase 6-A)
// 입력 파일 경로 + 적용할 자동화 프로젝트 규칙을 하나의 작업으로 묶는다.
// ============================================================
class BatchJob
{
public:
    BatchJob() {}

    BatchJob(const CString& strInputPath, const AutomationProject& project)
        : m_strInputPath(strInputPath)
        , m_project(project)
    {}

    CString               GetInputPath() const { return m_strInputPath; }
    const AutomationProject& GetProject() const { return m_project; }

    void SetInputPath(const CString& v)        { m_strInputPath = v; }
    void SetProject(const AutomationProject& v) { m_project = v; }

private:
    CString          m_strInputPath;
    AutomationProject m_project;
};
