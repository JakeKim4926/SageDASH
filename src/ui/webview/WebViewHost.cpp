#include "pch.h"
#include "WebViewHost.h"

#include <WebView2.h>
#include <wrl.h>

#include "SageMgr.h"
#include "WebBridgeMessage.h"

BEGIN_MESSAGE_MAP(WebViewHost, CWnd)
    ON_WM_SIZE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

// ---------------------------------------------------------------
// 생성 / 소멸
// ---------------------------------------------------------------

WebViewHost::WebViewHost()
    : m_pController(nullptr)
    , m_pWebView(nullptr)
    , m_bInitializing(FALSE)
    , m_bInitialized(FALSE)
    , m_bInvalidated(FALSE)
{
}

WebViewHost::~WebViewHost()
{
    if (m_pController != nullptr || m_pWebView != nullptr) {
        Destroy();
    }
}

// ---------------------------------------------------------------
// 공개 인터페이스
// ---------------------------------------------------------------

BOOL WebViewHost::Create(CWnd* pParent, const CRect& rect, UINT nID)
{
    LPCTSTR pszClassName = AfxRegisterWndClass(
        0,
        ::LoadCursor(nullptr, IDC_ARROW),
        (HBRUSH)(COLOR_WINDOW + 1),
        nullptr);

    if (!CWnd::CreateEx(0, pszClassName, _T(""), WS_CHILD | WS_VISIBLE,
        rect, pParent, nID)) {
        sageMgr.Log(_T("[WebViewHost] 호스트 윈도우 생성 실패"));
        return FALSE;
    }

    StartAsyncInit();
    return TRUE;
}

void WebViewHost::Destroy()
{
    m_bInvalidated  = TRUE;
    m_bInitializing = FALSE;
    m_bInitialized  = FALSE;

    if (m_pController != nullptr) {
        m_pController->Close();
        m_pController->Release();
        m_pController = nullptr;
    }

    if (m_pWebView != nullptr) {
        m_pWebView->Release();
        m_pWebView = nullptr;
    }
}

void WebViewHost::Resize(const CRect& rect)
{
    if (GetSafeHwnd() != nullptr) {
        MoveWindow(rect);
        // bounds 갱신은 MoveWindow 가 유발하는 WM_SIZE → OnSize 에서 처리
    }
}

BOOL WebViewHost::IsInitialized() const
{
    return m_bInitialized;
}

BOOL WebViewHost::IsInitializing() const
{
    return m_bInitializing;
}

void WebViewHost::PostWebMessage(const CString& strJson)
{
    if (!m_bInitialized || m_pWebView == nullptr) {
        return;
    }
    m_pWebView->PostWebMessageAsJson(strJson.GetString());
}

// ---------------------------------------------------------------
// 비동기 초기화
// ---------------------------------------------------------------

void WebViewHost::StartAsyncInit()
{
    m_bInitializing = TRUE;
    sageMgr.Log(_T("[WebViewHost] WebView2 비동기 초기화 시작"));

    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
        nullptr, nullptr, nullptr,
        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this](HRESULT hrEnv, ICoreWebView2Environment* pEnv) -> HRESULT {
                if (m_bInvalidated) {
                    return S_OK;
                }

                if (FAILED(hrEnv) || pEnv == nullptr) {
                    m_bInitializing = FALSE;
                    sageMgr.Log(_T("[WebViewHost] WebView2 환경 생성 실패"));
                    return S_OK;
                }

                HRESULT hrCtrl = pEnv->CreateCoreWebView2Controller(
                    GetSafeHwnd(),
                    Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [this](HRESULT hrController, ICoreWebView2Controller* pController) -> HRESULT {
                            if (m_bInvalidated) {
                                return S_OK;
                            }

                            if (FAILED(hrController) || pController == nullptr) {
                                m_bInitializing = FALSE;
                                sageMgr.Log(_T("[WebViewHost] WebView2 컨트롤러 생성 실패"));
                                return S_OK;
                            }

                            OnInitCompleted(pController);
                            return S_OK;
                        }).Get());

                if (FAILED(hrCtrl)) {
                    m_bInitializing = FALSE;
                    sageMgr.Log(_T("[WebViewHost] CreateCoreWebView2Controller 호출 실패"));
                }
                return S_OK;
            }).Get());

    if (FAILED(hr)) {
        m_bInitializing = FALSE;
        sageMgr.Log(_T("[WebViewHost] CreateCoreWebView2EnvironmentWithOptions 호출 실패"));
    }
}

void WebViewHost::OnInitCompleted(ICoreWebView2Controller* pController)
{
    m_pController = pController;
    m_pController->AddRef();

    HRESULT hr = m_pController->get_CoreWebView2(&m_pWebView);
    if (FAILED(hr) || m_pWebView == nullptr) {
        m_pController->Release();
        m_pController = nullptr;
        m_bInitializing = FALSE;
        sageMgr.Log(_T("[WebViewHost] CoreWebView2 인터페이스 취득 실패"));
        return;
    }

    CRect rect;
    GetClientRect(&rect);
    RECT bounds = { rect.left, rect.top, rect.right, rect.bottom };
    m_pController->put_Bounds(bounds);
    m_pController->put_IsVisible(TRUE);

    m_bInitializing = FALSE;
    m_bInitialized  = TRUE;
    sageMgr.Log(_T("[WebViewHost] WebView2 초기화 완료"));

    PostWebMessage(WebBridgeMessage::BuildNativeReady());
}

// ---------------------------------------------------------------
// 메시지 핸들러
// ---------------------------------------------------------------

void WebViewHost::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    if (m_bInitialized && m_pController != nullptr) {
        CRect rect;
        GetClientRect(&rect);
        RECT bounds = { rect.left, rect.top, rect.right, rect.bottom };
        m_pController->put_Bounds(bounds);
    }
}

void WebViewHost::OnDestroy()
{
    Destroy();
    CWnd::OnDestroy();
}
