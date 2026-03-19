
#pragma once

#include "IActionHandler.h"

// ============================================================
// EmailActionHandler — Simple MAPI 기반 이메일 발송 액션
// 현재 데이터를 CSV 임시 파일로 첨부하여
// 기본 메일 클라이언트의 작성 창을 엽니다.
// ============================================================
class EmailActionHandler : public IActionHandler
{
public:
    CString GetName() const override;
    void Execute(const TabularData& data) override;

private:
    static CString WriteTempCsv(const TabularData& data);
    static void    LaunchMailCompose(const CString& strAttachPath,
                                     const CString& strSubject);
};
