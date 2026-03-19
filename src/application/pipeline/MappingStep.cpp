
#include "pch.h"
#include "framework.h"
#include "MappingStep.h"

CString MappingStep::GetName() const
{
    return _T("컬럼 매핑");
}

BOOL MappingStep::Execute(ExecutionContext& ctx)
{
    if (ctx.m_arrMappingRules.empty()) {
        ctx.m_strLog += _T("[매핑] 규칙 없음 — 원본 데이터 그대로 사용\r\n");
        return TRUE;
    }

    const DataSheet& src = *ctx.m_pSourceSheet;
    if (src.GetRowCount() == 0) {
        ctx.m_strLog += _T("[매핑] 오류: 소스 시트가 비어있습니다.\r\n");
        return FALSE;
    }

    const std::vector<CString>& header = src.m_arrRows[0];

    // 필수 컬럼 누락 검사
    for (int i = 0; i < (int)ctx.m_arrMappingRules.size(); i++) {
        const MappingRule& rule = ctx.m_arrMappingRules[i];
        if (!rule.IsRequired())
            continue;
        if (FindColumnIndex(header, rule.GetSourceColumn()) < 0) {
            CString strMsg;
            strMsg.Format(_T("[매핑] 오류: 필수 컬럼 '%s' 을 찾을 수 없습니다.\r\n"),
                (LPCTSTR)rule.GetSourceColumn());
            ctx.m_strLog += strMsg;
            return FALSE;
        }
    }

    // 변환 시트 구성
    DataSheet& out = ctx.m_transformedSheet;
    out.m_strName = src.m_strName;
    out.m_arrRows.clear();

    // 헤더 행 생성
    std::vector<CString> outHeader;
    std::vector<int>     arrColIdx;
    for (int i = 0; i < (int)ctx.m_arrMappingRules.size(); i++) {
        const MappingRule& rule = ctx.m_arrMappingRules[i];
        int nIdx = FindColumnIndex(header, rule.GetSourceColumn());
        if (nIdx >= 0) {
            outHeader.push_back(rule.GetTargetColumn());
            arrColIdx.push_back(nIdx);
        }
    }
    out.m_arrRows.push_back(outHeader);

    // 데이터 행 변환
    int nDataRows = src.GetRowCount();
    for (int r = 1; r < nDataRows; r++) {
        // 100행마다 취소 요청 확인
        if (r % 100 == 0 && ctx.IsCancelled()) {
            ctx.m_strLog += _T("[매핑] 취소됨\r\n");
            return FALSE;
        }
        const std::vector<CString>& srcRow = src.m_arrRows[r];
        std::vector<CString> outRow;
        for (int c = 0; c < (int)arrColIdx.size(); c++) {
            int nSrcCol = arrColIdx[c];
            outRow.push_back(nSrcCol < (int)srcRow.size() ? srcRow[nSrcCol] : CString());
        }
        out.m_arrRows.push_back(outRow);
    }

    CString strMsg;
    strMsg.Format(_T("[매핑] 완료 — %d개 규칙 적용, %d행 변환\r\n"),
        (int)arrColIdx.size(), nDataRows - 1);
    ctx.m_strLog += strMsg;
    return TRUE;
}

int MappingStep::FindColumnIndex(const std::vector<CString>& header, const CString& strColumn) const
{
    for (int i = 0; i < (int)header.size(); i++) {
        if (header[i] == strColumn)
            return i;
    }
    return -1;
}
