
#include "pch.h"
#include "framework.h"
#include "ProjectSerializer.h"
#include "SageException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const int PROJECT_VERSION = 2;  // v2: [actions] 섹션 추가

// \\n → \n, \\\\ → \\ 로 이스케이프하여 단일 줄 저장
CString ProjectSerializer::EscapeValue(const CString& str)
{
    CString out = str;
    out.Replace(_T("\\"), _T("\\\\"));
    out.Replace(_T("\r\n"), _T("\\n"));
    out.Replace(_T("\n"), _T("\\n"));
    return out;
}

CString ProjectSerializer::UnescapeValue(const CString& str)
{
    CString out;
    for (int i = 0; i < str.GetLength(); i++) {
        if (str[i] == _T('\\') && i + 1 < str.GetLength()) {
            TCHAR next = str[i + 1];
            if (next == _T('n')) {
                out += _T('\n');
                i++;
                continue;
            }
            if (next == _T('\\')) {
                out += _T('\\');
                i++;
                continue;
            }
        }
        out += str[i];
    }
    return out;
}

void ProjectSerializer::Save(const AutomationProject& project, const CString& strFilePath)
{
    CStdioFile file;
    if (!file.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
        CString msg;
        msg.Format(_T("프로젝트 파일을 저장할 수 없습니다: %s"), (LPCTSTR)strFilePath);
        throw SageException(msg);
    }

    // [meta]
    file.WriteString(_T("[meta]\r\n"));
    CString line;
    line.Format(_T("version=%d\r\n"), PROJECT_VERSION);
    file.WriteString(line);
    file.WriteString(_T("\r\n"));

    // [output]
    file.WriteString(_T("[output]\r\n"));
    file.WriteString(_T("path=") + EscapeValue(project.m_strOutputPath) + _T("\r\n"));
    file.WriteString(_T("\r\n"));

    // [mapping]
    file.WriteString(_T("[mapping]\r\n"));
    line.Format(_T("count=%d\r\n"), (int)project.m_arrMappingRules.size());
    file.WriteString(line);
    for (int i = 0; i < (int)project.m_arrMappingRules.size(); i++) {
        const MappingRule& rule = project.m_arrMappingRules[i];
        CString prefix;
        prefix.Format(_T("%d."), i);
        file.WriteString(prefix + _T("source=") + EscapeValue(rule.GetSourceColumn()) + _T("\r\n"));
        file.WriteString(prefix + _T("target=") + EscapeValue(rule.GetTargetColumn()) + _T("\r\n"));
        line.Format(_T("%d.required=%d\r\n"), i, rule.IsRequired() ? 1 : 0);
        file.WriteString(line);
    }
    file.WriteString(_T("\r\n"));

    // [validation]
    file.WriteString(_T("[validation]\r\n"));
    line.Format(_T("count=%d\r\n"), (int)project.m_arrValidationRules.size());
    file.WriteString(line);
    for (int i = 0; i < (int)project.m_arrValidationRules.size(); i++) {
        const ValidationRule& rule = project.m_arrValidationRules[i];
        CString prefix;
        prefix.Format(_T("%d."), i);
        file.WriteString(prefix + _T("column=") + EscapeValue(rule.GetColumnName()) + _T("\r\n"));
        line.Format(_T("%d.type=%d\r\n"), i, (int)rule.GetType());
        file.WriteString(line);
        line.Format(_T("%d.severity=%d\r\n"), i, (int)rule.GetSeverity());
        file.WriteString(line);
        file.WriteString(prefix + _T("pattern=") + EscapeValue(rule.GetPattern()) + _T("\r\n"));
    }
    file.WriteString(_T("\r\n"));

    // [actions]
    file.WriteString(_T("[actions]\r\n"));
    line.Format(_T("count=%d\r\n"), (int)project.m_arrActionDefs.size());
    file.WriteString(line);
    for (int i = 0; i < (int)project.m_arrActionDefs.size(); i++) {
        const ActionDefinition& def = project.m_arrActionDefs[i];
        CString prefix;
        prefix.Format(_T("%d."), i);
        line.Format(_T("%d.type=%d\r\n"), i, (int)def.m_type);
        file.WriteString(line);
        file.WriteString(prefix + _T("param1=") + EscapeValue(def.m_strParam1) + _T("\r\n"));
        file.WriteString(prefix + _T("param2=") + EscapeValue(def.m_strParam2) + _T("\r\n"));
        file.WriteString(prefix + _T("param3=") + EscapeValue(def.m_strParam3) + _T("\r\n"));
        file.WriteString(prefix + _T("param4=") + EscapeValue(def.m_strParam4) + _T("\r\n"));
        file.WriteString(prefix + _T("param5=") + EscapeValue(def.m_strParam5) + _T("\r\n"));
        file.WriteString(prefix + _T("param6=") + EscapeValue(def.m_strParam6) + _T("\r\n"));
    }

    file.Close();
}

void ProjectSerializer::Load(const CString& strFilePath, AutomationProject& outProject)
{
    CStdioFile file;
    if (!file.Open(strFilePath, CFile::modeRead | CFile::typeText)) {
        CString msg;
        msg.Format(_T("프로젝트 파일을 열 수 없습니다: %s"), (LPCTSTR)strFilePath);
        throw SageException(msg);
    }

    outProject.Clear();

    CString strLine;
    CString strSection;
    int nMappingCount    = 0;
    int nValidationCount = 0;
    int nActionCount     = 0;

    std::vector<MappingRule>      arrMapping;
    std::vector<ValidationRule>   arrValidation;
    std::vector<ActionDefinition> arrActions;

    while (file.ReadString(strLine)) {
        strLine.TrimRight(_T("\r\n"));
        strLine.Trim();

        if (strLine.IsEmpty())
            continue;

        // 섹션 헤더
        if (strLine.GetLength() >= 2 && strLine[0] == _T('[')) {
            int nClose = strLine.Find(_T(']'));
            if (nClose > 0)
                strSection = strLine.Mid(1, nClose - 1);
            continue;
        }

        // key=value
        int nEq = strLine.Find(_T('='));
        if (nEq < 0)
            continue;
        CString key   = strLine.Left(nEq);
        CString value = UnescapeValue(strLine.Mid(nEq + 1));

        if (strSection == _T("output")) {
            if (key == _T("path"))
                outProject.m_strOutputPath = value;
        } else if (strSection == _T("mapping")) {
            if (key == _T("count")) {
                nMappingCount = _ttoi(value);
                arrMapping.resize(nMappingCount);
            } else {
                int nDot = key.Find(_T('.'));
                if (nDot > 0) {
                    int nIdx   = _ttoi(key.Left(nDot));
                    CString sub = key.Mid(nDot + 1);
                    if (nIdx >= 0 && nIdx < nMappingCount) {
                        if (sub == _T("source"))   arrMapping[nIdx].SetSourceColumn(value);
                        else if (sub == _T("target"))   arrMapping[nIdx].SetTargetColumn(value);
                        else if (sub == _T("required")) arrMapping[nIdx].SetRequired(_ttoi(value) != 0);
                    }
                }
            }
        } else if (strSection == _T("validation")) {
            if (key == _T("count")) {
                nValidationCount = _ttoi(value);
                arrValidation.resize(nValidationCount);
            } else {
                int nDot = key.Find(_T('.'));
                if (nDot > 0) {
                    int nIdx   = _ttoi(key.Left(nDot));
                    CString sub = key.Mid(nDot + 1);
                    if (nIdx >= 0 && nIdx < nValidationCount) {
                        if (sub == _T("column"))
                            arrValidation[nIdx].SetColumnName(value);
                        else if (sub == _T("type"))
                            arrValidation[nIdx].SetType((ValidationType)_ttoi(value));
                        else if (sub == _T("severity"))
                            arrValidation[nIdx].SetSeverity((ValidationSeverity)_ttoi(value));
                        else if (sub == _T("pattern"))
                            arrValidation[nIdx].SetPattern(value);
                    }
                }
            }
        } else if (strSection == _T("actions")) {
            if (key == _T("count")) {
                nActionCount = _ttoi(value);
                arrActions.resize(nActionCount);
            } else {
                int nDot = key.Find(_T('.'));
                if (nDot > 0) {
                    int nIdx    = _ttoi(key.Left(nDot));
                    CString sub = key.Mid(nDot + 1);
                    if (nIdx >= 0 && nIdx < nActionCount) {
                        if (sub == _T("type"))        arrActions[nIdx].m_type = (ActionType)_ttoi(value);
                        else if (sub == _T("param1")) arrActions[nIdx].m_strParam1 = value;
                        else if (sub == _T("param2")) arrActions[nIdx].m_strParam2 = value;
                        else if (sub == _T("param3")) arrActions[nIdx].m_strParam3 = value;
                        else if (sub == _T("param4")) arrActions[nIdx].m_strParam4 = value;
                        else if (sub == _T("param5")) arrActions[nIdx].m_strParam5 = value;
                        else if (sub == _T("param6")) arrActions[nIdx].m_strParam6 = value;
                    }
                }
            }
        }
    }

    file.Close();

    outProject.m_arrMappingRules    = arrMapping;
    outProject.m_arrValidationRules = arrValidation;
    outProject.m_arrActionDefs      = arrActions;
}
