
#pragma once

#include "IOutputWriter.h"

// ============================================================
// PdfOutputWriter — TabularData → PDF 파일 저장 (PDF 1.4 raw)
// 외부 라이브러리 없이 PDF 사양 직접 구현
// ============================================================
class PdfOutputWriter : public IOutputWriter
{
public:
    void Write(const TabularData& data, const IOutputTarget& target) override;
};
