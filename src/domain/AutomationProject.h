
#pragma once

#include "MappingRule.h"
#include "ValidationRule.h"
#include "ActionDefinition.h"
#include <vector>

// ============================================================
// AutomationProject — 자동화 작업 세션의 규칙 모음
// 매핑/검증 규칙, 출력 경로, 후속 액션을 하나의 프로젝트(.sagep)로 저장한다.
// Phase 3: 매핑/검증/출력 경로
// Phase 5: 후속 액션 목록 추가
// ============================================================
class AutomationProject
{
public:
    AutomationProject() {}

    std::vector<MappingRule>    m_arrMappingRules;
    std::vector<ValidationRule> m_arrValidationRules;
    std::vector<ActionDefinition> m_arrActions;
    CString                     m_strOutputPath;

    BOOL IsEmpty() const {
        return m_arrMappingRules.empty()
            && m_arrValidationRules.empty()
            && m_arrActions.empty();
    }

    void Clear() {
        m_arrMappingRules.clear();
        m_arrValidationRules.clear();
        m_arrActions.clear();
        m_strOutputPath.Empty();
    }
};
