
#include "pch.h"
#include "framework.h"
#include "ValidationService.h"

ValidationResult ValidationService::Validate(const DataSheet& sheet,
                                             const std::vector<ValidationRule>& arrRules) const
{
    ValidationResult result;

    if (sheet.GetRowCount() < 2 || arrRules.empty())
        return result;

    const std::vector<CString>& header = sheet.m_arrRows[0];
    int nColCount = (int)header.size();

    for (int i = 0; i < (int)arrRules.size(); i++) {
        const ValidationRule& rule = arrRules[i];

        int nColIdx = -1;
        for (int c = 0; c < nColCount; c++) {
            if (header[c] == rule.GetColumnName()) {
                nColIdx = c;
                break;
            }
        }
        if (nColIdx < 0)
            continue;

        int nDataRows = sheet.GetRowCount();
        for (int r = 1; r < nDataRows; r++) {
            const std::vector<CString>& row = sheet.m_arrRows[r];
            CString cell = (nColIdx < (int)row.size()) ? row[nColIdx] : CString();

            if (rule.GetType() == VALIDATION_REQUIRED) {
                CString trimmed = cell;
                trimmed.Trim();
                if (trimmed.IsEmpty()) {
                    CString msg;
                    if (!rule.GetErrorMsg().IsEmpty()) {
                        msg = rule.GetErrorMsg();
                    } else {
                        msg.Format(_T("[%s] \xD544\xC218\xAC12 \xB204\xB77D"), (LPCTSTR)rule.GetColumnName());
                    }
                    result.AddIssue(ValidationIssue(r, rule.GetColumnName(), msg, rule.GetSeverity()));
                }
            } else if (rule.GetType() == VALIDATION_FORMAT) {
                CString pattern = rule.GetPattern();
                if (!pattern.IsEmpty() && cell.Find(pattern) < 0) {
                    CString msg;
                    if (!rule.GetErrorMsg().IsEmpty()) {
                        msg = rule.GetErrorMsg();
                    } else {
                        msg.Format(_T("[%s] \xD3EC\xB9F7 \xBD88\xC77C\xCE58"), (LPCTSTR)rule.GetColumnName());
                    }
                    result.AddIssue(ValidationIssue(r, rule.GetColumnName(), msg, rule.GetSeverity()));
                }
            }
        }
    }

    return result;
}
