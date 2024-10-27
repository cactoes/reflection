#include <core/webview_browser_api.hpp>
#include <reflection/utils.hpp>
#include <ext/core_ui.hpp>

#include <shlobj.h>
#include <filesystem>
#include <string>
#include <iostream>

using Microsoft::WRL::Make, Microsoft::WRL::Callback, Microsoft::WRL::ComPtr;

// TODO: move
int browse_callback_proc(HWND hwnd, UINT uMsg, LPARAM, LPARAM lpData) {
    if (uMsg == BFFM_INITIALIZED)
        SendMessageA(hwnd, BFFM_SETSELECTION, TRUE, lpData);

    return 0;
}

// TODO: move
std::optional<std::any> webview_browser_api::handle_window_events(abstract_browser_api*, const std::vector<js_value>& args) {
    const auto message_type = args.at(0).as<std::string>();

    switch (ru_hash_fnv1a_64(message_type.c_str())) {
        case ru_hash_fnv1a_64("we_move_window"): {
            RECT rect;
            GetWindowRect(m_window, &rect);

            int newLeft = rect.left + args.at(1).as<int>();
            int newTop = rect.top + args.at(2).as<int>();
            int newWidth = rect.right - rect.left;
            int newHeight = rect.bottom - rect.top;

            return (bool)SetWindowPos(m_window, nullptr, newLeft, newTop,
                newWidth, newHeight, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        case ru_hash_fnv1a_64("we_open_folder_selector"): {
            char path[MAX_PATH];
            auto str = args.at(1).as<std::string>();
            const char* path_param = str.c_str();

            BROWSEINFOA bi = { 0 };
            bi.lpszTitle = "Select folder";
            bi.ulFlags = BIF_RETURNONLYFSDIRS;
            bi.lpfn = browse_callback_proc;
            bi.lParam = (LPARAM)path_param;

            LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);

            if (pidl != 0) {
                SHGetPathFromIDListA(pidl, path);
                CoTaskMemFree(pidl);
                return std::string(path);
            }

            return "";
        }
        case ru_hash_fnv1a_64("we_minimize"):
            return (bool)ShowWindow(m_window, SW_MINIMIZE);
        case ru_hash_fnv1a_64("we_resize"):
            if (WINDOWPLACEMENT wp { .length = sizeof(WINDOWPLACEMENT) }; GetWindowPlacement(m_window, &wp))
                return ShowWindow(m_window, wp.showCmd != SW_SHOWMAXIMIZED ? SW_SHOWMAXIMIZED : SW_RESTORE);
            return false;
        case ru_hash_fnv1a_64("we_close"):
            return (bool)DestroyWindow(m_window);
        default:
            return std::nullopt;
    }
}

HWND webview_browser_api::get_window_handle() {
    return m_window;
}

webview_browser_api::webview_browser_api(HWND window) {
    m_window = window;
    SetWindowLongPtrA(m_window, GWLP_USERDATA, (LONG_PTR)this);

    register_function("handle_window_events", &webview_browser_api::handle_window_events, this);

    char temp_path[MAX_PATH];
    GetTempPathA(MAX_PATH, temp_path);
    m_cache_path = temp_path;
    m_cache_path /= "reflection-webview-cache";
}

void webview_browser_api::start() {
    const auto cache_path_str = get_cache_path().string();
    CreateDirectoryA(cache_path_str.c_str(), nullptr);

    ComPtr<CoreWebView2EnvironmentOptions> options = Make<CoreWebView2EnvironmentOptions>();
    options->put_AdditionalBrowserArguments(L"--disable-web-security");

    CreateCoreWebView2EnvironmentWithOptions(nullptr, ru_str_to_wstr(cache_path_str).c_str(), options.Get(),
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>([this](HRESULT, ICoreWebView2Environment* environment) -> HRESULT {
            return environment->CreateCoreWebView2Controller(m_window,
            Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                [this](HRESULT, ICoreWebView2Controller* controller) -> HRESULT {
                    if (controller != nullptr) {
                        m_webview_controller = controller;
                        m_webview_controller->get_CoreWebView2(&m_webview2);
                    }

                    wil::com_ptr<ICoreWebView2Settings> comptr_settings;
                    m_webview2->get_Settings(&comptr_settings);
                    comptr_settings->put_IsScriptEnabled(TRUE);
                    comptr_settings->put_IsWebMessageEnabled(TRUE);
                    comptr_settings->put_AreDevToolsEnabled(REFLECTION_DBG);

                    wil::com_ptr<ICoreWebView2_13> comptr_webbiew2_13;
                    wil::com_ptr<ICoreWebView2Profile> comptr_webview2profile;
                    wil::com_ptr<ICoreWebView2Profile2> comptr_webview2profile2;

                    if (!FAILED(m_webview2->QueryInterface(IID_PPV_ARGS(&comptr_webbiew2_13)))) {
                        if (!FAILED(comptr_webbiew2_13->get_Profile(&comptr_webview2profile))) {
                            if (!FAILED(comptr_webview2profile->QueryInterface(IID_PPV_ARGS(&comptr_webview2profile2)))) {
                                comptr_webview2profile2->ClearBrowsingData(COREWEBVIEW2_BROWSING_DATA_KINDS_FILE_SYSTEMS |
                                    COREWEBVIEW2_BROWSING_DATA_KINDS_INDEXED_DB | COREWEBVIEW2_BROWSING_DATA_KINDS_WEB_SQL |
                                    COREWEBVIEW2_BROWSING_DATA_KINDS_CACHE_STORAGE | COREWEBVIEW2_BROWSING_DATA_KINDS_ALL_DOM_STORAGE |
                                    COREWEBVIEW2_BROWSING_DATA_KINDS_ALL_SITE | COREWEBVIEW2_BROWSING_DATA_KINDS_DISK_CACHE |
                                    COREWEBVIEW2_BROWSING_DATA_KINDS_DOWNLOAD_HISTORY | COREWEBVIEW2_BROWSING_DATA_KINDS_GENERAL_AUTOFILL |
                                    COREWEBVIEW2_BROWSING_DATA_KINDS_BROWSING_HISTORY | COREWEBVIEW2_BROWSING_DATA_KINDS_SETTINGS |
                                    COREWEBVIEW2_BROWSING_DATA_KINDS_ALL_PROFILE, nullptr);
                            }
                        }
                    }

                    if (!FAILED(m_webview2->QueryInterface(IID_PPV_ARGS(&m_webview2_2)))) {
                        m_webview2_2->add_DOMContentLoaded(
                            Callback<ICoreWebView2DOMContentLoadedEventHandler>(
                                [this](ICoreWebView2*, ICoreWebView2DOMContentLoadedEventArgs*) -> HRESULT {
                                    m_webview2->ExecuteScript(ru_str_to_wstr(format_byte_array(core_ui)).c_str(), nullptr);
                                    return S_OK;
                                }
                            ).Get(), nullptr
                        );
                    }

                    RECT bounds;
                    GetClientRect(m_window, &bounds);
                    m_webview_controller->put_Bounds(bounds);

                    m_webview2->add_WebMessageReceived(
                        Callback<ICoreWebView2WebMessageReceivedEventHandler>(
                            [this](ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
                                wil::unique_cotaskmem_string comptrWebMessageRaw;
                                args->get_WebMessageAsJson(&comptrWebMessageRaw);

                                const nlohmann::json web_message_json = nlohmann::json::parse(ru_wstr_to_str(comptrWebMessageRaw.get()));

                                const auto result = m_browser_interface.parse_message(web_message_json);
                                
                                if (m_functions.contains(result.function)) {
                                    const auto function_result = m_functions.at(result.function)(this, result.args);
                                    const auto result_script = m_browser_interface.create_result(result.function, function_result.value_or(std::any{}));

                                    return sender->ExecuteScript(ru_str_to_wstr(result_script).c_str(), nullptr);
                                }

                                if (REFLECTION_DBG)
                                    std::cerr << "[reflection::webview_browser_api] function: \"" << result.function << "\" not found!" << "\n";

                                return S_OK;
                            }
                        ).Get(), nullptr
                    );
                    
                    m_webview2->ExecuteScript(ru_str_to_wstr(format_byte_array(core_ui)).c_str(), nullptr);
                    m_is_ready = true;
                    return S_OK;
                }
            ).Get());
        }).Get()
    );
}

const std::filesystem::path& webview_browser_api::get_cache_path() {
    return m_cache_path;
}

void webview_browser_api::execute_event(const browser_event_t& event) {
    if (m_thread_created_id != std::this_thread::get_id() && REFLECTION_DBG)
        std::cerr << "[reflection::webview_browser_api] thread mismatch! unable to call function: \"" <<  event.name << "\"\n";

    auto script = m_browser_interface.create_function_call(event);
    m_webview2->ExecuteScript(ru_str_to_wstr(script).c_str(), nullptr);
    //  m_webview2->PostWebMessageAsJson();
}

void webview_browser_api::set_title(const std::string& title) {
    call_function("__set_title_wrapper__", { title });
}

void webview_browser_api::set_icon(const std::string& base64_data) {
    call_function("__set_icon_wrapper__", { base64_data });
}

void webview_browser_api::set_color(const std::string& color) {
    call_function("__set_color_wrapper__", { color });
}

void webview_browser_api::set_bounds(const RECT& bounds) const {
    if (m_webview_controller)
        m_webview_controller->put_Bounds(bounds);
}