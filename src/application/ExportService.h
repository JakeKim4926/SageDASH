
#pragma once

#include "TabularData.h"

// ============================================================
// ExportService — 파일 확장자에 따라 CSV/XLSX 저장 (Phase 3)
// ============================================================
class ExportService
{
public:
    // 저장 성공 시 빈 CString, 실패 시 오류 메시지 반환
    CString Export(const TabularData& data, const CString& strFilePath) const;
};
