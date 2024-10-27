#include <reflection/abstract_browser_api.hpp>

void abstract_browser_api::handle_events() {
    if (!m_is_ready)
        return;

    browser_event_t event = {};
    while (m_event_handler.get(event))
        execute_event(event);
}

void abstract_browser_api::register_function(const std::string& name, browser_function_t func) {
    m_functions[name] = func;
}

void abstract_browser_api::call_function(const std::string& name, const std::vector<std::any>& args) {
    if (m_thread_created_id == std::this_thread::get_id()) {
        execute_event(browser_event_t{ name, args });
    } else {
        m_event_handler.post(browser_event_t{ name, args });
        PostMessageA(get_window_handle(), WM_HANDLE_BROWSER_EVENTS, 0, 0);
    }
}

// // ========================================= d2d1 render code =========================================
// /*
// #define WIN32_LEAN_AND_MEAN
// #include <Windows.h>
// #include <string>
// #include <d2d1.h>
// #pragma comment(lib, "d2d1")

// template <typename _d2d1_ptr_type>
// class safe_ptr {
// public:
//     safe_ptr(_d2d1_ptr_type* ptr = nullptr) { assing(ptr); }
//     ~safe_ptr() { discard(); }

//     bool discard() {
//         if (m_ptr) {
//             ((IUnknown*)m_ptr)->Release();
//             m_ptr = nullptr;
//             return true;
//         }

//         return false;
//     }

//     void assing(_d2d1_ptr_type* ptr) {
//         m_ptr = ptr;
//     }

//     bool is_valid() const {
//         return m_ptr != nullptr;
//     }

//     _d2d1_ptr_type* get() {
//         return m_ptr;
//     }

//     _d2d1_ptr_type** operator&() {
//         return &m_ptr;
//     }

//     _d2d1_ptr_type* operator->() {
//         return m_ptr;
//     }

//     bool operator bool() {
//         return is_valid();
//     }

// private:
//     _d2d1_ptr_type* m_ptr = nullptr;
// };

// // bool release_ptr(IUnknown* ptr) {
// //     if (ptr) {
// //         (ptr)->Release();
// //         ptr = nullptr;
// //         return true;
// //     }

// //     return false;
// // }

// class d2d1_render_api {
// public:
//     bool init() {
//         return SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_factory));
//     }

//     bool create_target() {
//         if (m_render_target)
//             return true;

//         RECT rc;
//         GetClientRect(m_window, &rc);

//         D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
        
//         HRESULT result = m_factory->CreateHwndRenderTarget(
//             D2D1::RenderTargetProperties(),
//             D2D1::HwndRenderTargetProperties(m_window, size),
//             &m_render_target
//         );

//         return SUCCEEDED(result);
//     }

//     void discard_target() {
//         m_render_target.discard();
//     }

//     ID2D1HwndRenderTarget* const get_render_target() {
//         return m_render_target.get();
//     }

//     void set_window(HWND window) {
//         m_window = window;
//     }

// private:
//     safe_ptr<ID2D1Factory> m_factory {};
//     safe_ptr<ID2D1HwndRenderTarget> m_render_target {};
//     HWND m_window = nullptr;
// };

// d2d1_render_api api {};

// // ID2D1Factory*          pFactory = nullptr;
// // ID2D1HwndRenderTarget* pRenderTarget = nullptr;
// // ID2D1SolidColorBrush*  pBrush = nullptr;
// // D2D1_ELLIPSE           ellipse {};
// // HWND                   ghWnd = nullptr;

// // template <class T>
// // void SafeRelease(T **ppT) {
// //     if (*ppT)
// //     {
// //         (*ppT)->Release();
// //         *ppT = NULL;
// //     }
// // }

// // void CalculateLayout() {
// //     if (pRenderTarget == nullptr)
// //         return;
// //     D2D1_SIZE_F size = pRenderTarget->GetSize();
// //     const float x = size.width / 2;
// //     const float y = size.height / 2;
// //     const float radius = min(x, y);
// //     ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
// // }

// // HRESULT CreateGraphicsResources() {
// //     HRESULT hr = S_OK;
// //     if (pRenderTarget == nullptr) {
// //         RECT rc;
// //         GetClientRect(ghWnd, &rc);

// //         D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

// //         hr = pFactory->CreateHwndRenderTarget(
// //             D2D1::RenderTargetProperties(),
// //             D2D1::HwndRenderTargetProperties(ghWnd, size),
// //             &pRenderTarget
// //         );

// //         if (SUCCEEDED(hr)) {
// //             const D2D1_COLOR_F color = D2D1::ColorF(1.f, 1.f, 0.f);
// //             hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

// //             if (SUCCEEDED(hr)) {
// //                 CalculateLayout();
// //             }
// //         }
// //     }

// //     return hr;
// // }

// // void DiscardGraphicsResources() {
// //     SafeRelease(&pRenderTarget);
// //     SafeRelease(&pBrush);
// // }

// // void OnPaint() {
// //     HRESULT hr = CreateGraphicsResources();
// //     if (SUCCEEDED(hr)) {
// //         PAINTSTRUCT ps;
// //         BeginPaint(ghWnd, &ps);
     
// //         pRenderTarget->BeginDraw();

// //         pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
// //         pRenderTarget->FillEllipse(ellipse, pBrush);

// //         hr = pRenderTarget->EndDraw();
// //         if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
// //             DiscardGraphicsResources();

// //         EndPaint(ghWnd, &ps);
// //     }
// // }

// // void Resize() {
// //     if (pRenderTarget != NULL) {
// //         RECT rc;
// //         GetClientRect(ghWnd, &rc);

// //         D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

// //         pRenderTarget->Resize(size);
// //         CalculateLayout();
// //         InvalidateRect(ghWnd, NULL, FALSE);
// //     }
// // }

// LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//     switch (uMsg) {
//         case WM_CREATE:
//             if (!api.init())
//                 return -1;
//             return 0;
//         case WM_DESTROY:
//             DiscardGraphicsResources();
//             SafeRelease(&pFactory);
//             PostQuitMessage(0);
//             return 0;
//         case WM_PAINT:
//             OnPaint();
//             return 0;
//         case WM_SIZE:
//             Resize();
//             return 0;
//         default:
//             return DefWindowProc(hWnd, uMsg, wParam, lParam);
//     }
// }

// // HWND WindowCreate(LPCSTR lpszWindowName) {
// //     LPCSTR lpszWindowClassName = "test WindowClass";

// //     WNDCLASS wc {};
// //     wc.lpfnWndProc = WndProc;
// //     wc.hInstance = GetModuleHandleA(nullptr);
// //     wc.lpszClassName = lpszWindowClassName;

// //     RegisterClassA(&wc);

// //     return CreateWindowExA(
// //         0, lpszWindowClassName, lpszWindowName, WS_OVERLAPPEDWINDOW,
// //         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
// //         nullptr, 0, wc.hInstance, nullptr
// //     );
// // }

// // int WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
// //     ghWnd = WindowCreate("Test window");

// //     ShowWindow(ghWnd, nCmdShow);

// //     MSG msg = { };
// //     while (GetMessageA(&msg, NULL, 0, 0))
// //     {
// //         TranslateMessage(&msg);
// //         DispatchMessageA(&msg);
// //     }

// //     return 0;
// // }
// */