#pragma once

// COM 인터페이스 전방 선언 (WebView2.h 는 .cpp 에서만 포함)
interface ICoreWebView2Controller;
interface ICoreWebView2;

// ---------------------------------------------------------------
// WebViewHost
//   CWnd 기반 WebView2 호스트 래퍼.
//   비동기 초기화(CreateCoreWebView2EnvironmentWithOptions) 완료 후
//   m_bInitialized 가 TRUE 로 전환된다.
//
//   수명 관리 원칙:
//   - 부모가 Destroy() 를 명시적으로 호출하거나, OnDestroy() 에서 자동 호출.
//   - Destroy() 는 m_bInvalidated 를 세팅해 잔여 비동기 콜백을 무효화한다.
// ---------------------------------------------------------------
class WebViewHost : public CWnd
{
public:
    WebViewHost();
    virtual ~WebViewHost();

    // 호스트 윈도우 생성 + 비동기 초기화 시작
    BOOL Create(CWnd* pParent, const CRect& rect, UINT nID);

    // WebView2 컨트롤러 해제 + 윈도우 파괴
    void Destroy();

    // 부모가 리사이즈될 때 호출 — 윈도우 이동 + bounds 갱신
    void Resize(const CRect& rect);

    BOOL IsInitialized() const;
    BOOL IsInitializing() const;

    // Phase 4-4 에서 본격 사용 — 초기화 전 호출 시 무시
    void PostWebMessage(const CString& strJson);

private:
    void StartAsyncInit();
    void OnInitCompleted(ICoreWebView2Controller* pController);

    ICoreWebView2Controller* m_pController;
    ICoreWebView2*           m_pWebView;
    BOOL                     m_bInitializing;
    BOOL                     m_bInitialized;
    BOOL                     m_bInvalidated;    // Destroy 후 잔여 콜백 무효화 플래그

protected:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();

    DECLARE_MESSAGE_MAP()
};
