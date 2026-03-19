
#pragma once

#include "MappingRule.h"
#include "ValidationRule.h"
#include "ActionDefinition.h"
#include <vector>

// ============================================================
// AutomationProject — 자동화 작업 세션의 규칙 모음 (Phase 3)
// 매핑/검증 규칙, 출력 경로, 후속 액션 정의를 하나의 프로젝트(.sagep)로 저장한다.
// ============================================================
class AutomationProject
{
public:
    AutomationProject() {}

    std::vector<MappingRule>      m_arrMappingRules;
    std::vector<ValidationRule>   m_arrValidationRules;
    CString                       m_strOutputPath;
    std::vector<ActionDefinition> m_arrActionDefs;  // 후속 액션 정의 목록 (Phase 5-C)

    BOOL IsEmpty() const {
        return m_arrMappingRules.empty() && m_arrValidationRules.empty();
    }

    void Clear() {
        m_arrMappingRules.clear();
        m_arrValidationRules.clear();
        m_strOutputPath.Empty();
        m_arrActionDefs.clear();
    }
};
