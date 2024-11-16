#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <reflection.hpp>
#include <map>
#include <memory>

#include "reflection/reflection.h"

std::map<HWND, std::unique_ptr<reflection::browser_window>> g_browser_windows = {};

#define BROWSER_WINDOW_CAST(handle) ((reflection::browser_window*)(handle))
#define COMPONENT_CAST(handle, type) ((reflection::component::##type*)(handle))

#define CHECK_HANDLE(handle, ...) \
    if ((handle) == nullptr) return __VA_ARGS__

#define RECAST_OPTIONS(opt, type) *((reflection::component::##type*)(opt))

#define UNWRAP_COMPONENT(comp) comp.value_or(nullptr).get()

#include <iostream>

REFLECTION_API void destroy_string(unsafe_string_t* ptr) {
    if (ptr) {
        if (ptr->string) {
            delete[] ptr->string;
            ptr->string = nullptr;
        }

        delete ptr;
    }
}

REFLECTION_API void destroy_array(unsafe_int_array_t* ptr) {
    if (ptr) {
        if (ptr->array) {
            delete[] ptr->array;
            ptr->array = nullptr;
        }

        delete ptr;
    }
}

LRESULT CALLBACK def_wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static int window_count = 0;

    if (reflection::handle_window_message(hWnd, msg, wParam, lParam))
        return TRUE;

    switch (msg) {
        case WM_HANDLE_WINDOW_CREATE:
            window_count++;
            return TRUE;
        case WM_CLOSE:
            DestroyWindow(hWnd);
            return TRUE;
        case WM_DESTROY:
            window_count--;
            if (window_count <= 0)
                PostQuitMessage(0);
            return TRUE;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

HWND create_window(const char* window_name, int width, int height) {
    // const HICON icon = LoadIconA(nullptr, MAKEINTRESOURCE(1));

    WNDCLASSEX window_class{};
    window_class.cbSize = sizeof(WNDCLASSEX);
    window_class.style = (CS_HREDRAW | CS_VREDRAW);
    window_class.lpfnWndProc = &def_wnd_proc;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = nullptr;
    window_class.hCursor = LoadCursorA(nullptr, IDC_ARROW);
    window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    window_class.lpszMenuName = nullptr;
    window_class.lpszClassName = "wnd_cls";
    // window_class.hIcon = icon;
    // window_class.hIconSm = icon;

    RegisterClassExA(&window_class);
    
    const DWORD window_style_flags = WS_POPUP | WS_VISIBLE;

    const int window_pos_x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    const int window_pos_y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

    const HWND window_handle = CreateWindowExA(
        0,
        "wnd_cls",
        window_name,
        window_style_flags,
        window_pos_x, window_pos_y,
        width, height,
        NULL, NULL,
        nullptr,
        NULL);

    ShowWindow(window_handle, SW_SHOW);
    UpdateWindow(window_handle);

    return window_handle;
}

REFLECTION_API browser_handle_t init_browser(const window_options_t* window_options, const frame_options_t* frame_options) {
    if (window_options == nullptr || frame_options == nullptr)
        return nullptr;

    HWND window_handle = create_window(window_options->name, window_options->width, window_options->height);
    
    std::unique_ptr<reflection::browser_window> sptr = std::make_unique<reflection::browser_window>(window_handle, RECAST_OPTIONS(frame_options, frame_options_t));
    reflection::browser_window* ptr = sptr.get();
    g_browser_windows[window_handle] = std::move(sptr);
    return (browser_handle_t)ptr;
}

REFLECTION_API void browser_window_start(browser_handle_t handle) {
    CHECK_HANDLE(handle);
    BROWSER_WINDOW_CAST(handle)->start();
}

REFLECTION_API component_frame_handle_t browser_window_get_frame(browser_handle_t handle) {
    CHECK_HANDLE(handle, nullptr);
    return (component_frame_handle_t)BROWSER_WINDOW_CAST(handle)->get_frame().get();
}

REFLECTION_API bool browser_window_is_ready(browser_handle_t handle) {
    CHECK_HANDLE(handle, false);
    return BROWSER_WINDOW_CAST(handle)->is_ready();
}

REFLECTION_API void browser_window_register_event_handler(browser_handle_t handle, event_t event, event_handler_t event_handler) {
    CHECK_HANDLE(handle);
    BROWSER_WINDOW_CAST(handle)->register_event_handler((reflection::event_t)event, [event_handler](auto handle) { event_handler((browser_handle_t)handle); });
}

REFLECTION_API void browser_window_set_icon(browser_handle_t handle, int resource_id) {
    CHECK_HANDLE(handle);
    BROWSER_WINDOW_CAST(handle)->set_icon(resource_id);
}

REFLECTION_API void browser_window_set_color(browser_handle_t handle, const char* color) {
    CHECK_HANDLE(handle);
    BROWSER_WINDOW_CAST(handle)->set_color(color);
}

REFLECTION_API void browser_window_close(browser_handle_t handle) {
    CHECK_HANDLE(handle);
    BROWSER_WINDOW_CAST(handle)->close();
}

REFLECTION_API void component_set_target(component_handle_t handle, const char* id) {
    CHECK_HANDLE(handle);
    COMPONENT_CAST(handle, abstract_component)->set_target(id);
}

REFLECTION_API void component_set_id(component_handle_t handle, const char* id) {
    CHECK_HANDLE(handle);
    COMPONENT_CAST(handle, abstract_component)->set_id(id);
}

REFLECTION_API void component_set_name(component_handle_t handle, const char* name) {
    CHECK_HANDLE(handle);
    COMPONENT_CAST(handle, abstract_component)->set_name(name);
}

REFLECTION_API unsafe_string_t* component_get_id(component_handle_t handle) {
    CHECK_HANDLE(handle, nullptr);

    auto str = COMPONENT_CAST(handle, abstract_component)->get_id();
    unsafe_string_t* result = new unsafe_string_t();

    result->string = new char[str.size() + 1];
    std::copy(str.begin(), str.end(), result->string);
    result->string[str.size()] = '\0';

    result->size = (int)(str.size() + 1ull);

    return result;
}

REFLECTION_API component_frame_handle_t component_frame_add_frame(component_frame_handle_t handle, const char* name, const frame_options_t* options) {
    CHECK_HANDLE(handle, nullptr);
    return (component_frame_handle_t)COMPONENT_CAST(handle, abstract_frame)->add_frame(name, RECAST_OPTIONS(options, frame_options_t)).get();
}

REFLECTION_API component_label_handle_t component_frame_add_label(component_frame_handle_t handle, const char* text) {
    CHECK_HANDLE(handle, nullptr);

    return (component_label_handle_t)UNWRAP_COMPONENT(COMPONENT_CAST(handle, abstract_frame)->add_label(text));
}

REFLECTION_API component_button_handle_t component_frame_add_button(component_frame_handle_t handle, const char* name, button_callback_t callback, button_options_t* options) {
    CHECK_HANDLE(handle, nullptr);

    return (component_button_handle_t)UNWRAP_COMPONENT(COMPONENT_CAST(handle, abstract_frame)->add_button(name, [callback](auto handle) {
        (*callback)((component_button_handle_t)handle);
    }, RECAST_OPTIONS(options, button_options_t)));
}

REFLECTION_API component_checkbox_handle_t component_frame_add_checkbox(component_frame_handle_t handle, const char* name, bool state, checkbox_callback_t callback) {
    CHECK_HANDLE(handle, nullptr);

    return (component_checkbox_handle_t)UNWRAP_COMPONENT(COMPONENT_CAST(handle, abstract_frame)->add_checkbox(name, state, [callback](auto handle, bool rt) {
        (*callback)((component_checkbox_handle_t)handle, rt);
    }));
}

REFLECTION_API component_selector_handle_t component_frame_add_selector(component_frame_handle_t handle, const char* name, const char* list_options[], int list_options_size, int index[], int index_size, selector_callback_t callback, selector_options_t* options) {
    CHECK_HANDLE(handle, nullptr);

    std::vector<std::string> vec_list_options(list_options, list_options + list_options_size);
    std::vector<int> vec_index(index, index + index_size);

    auto comp = COMPONENT_CAST(handle, abstract_frame)->add_selector(name, vec_list_options, vec_index, [callback](auto handle, std::vector<int> rt) {
        (*callback)((component_selector_handle_t)handle, rt.data(), (int)rt.size());
    }, RECAST_OPTIONS(options, selector_options_t));

    return (component_selector_handle_t)UNWRAP_COMPONENT(comp);
}

REFLECTION_API component_slider_handle_t component_frame_add_slider(component_frame_handle_t handle, const char* name, int _min, int _max, int current, slider_callback_t callback, slider_options_t* options) {
    CHECK_HANDLE(handle, nullptr);

    reflection::component::slider_options_t opts {};
    opts.show_ticks = options->show_ticks;
    opts.tick_name_left = options->tick_name_left;
    opts.tick_name_right = options->tick_name_right;

    return (component_slider_handle_t)UNWRAP_COMPONENT(COMPONENT_CAST(handle, abstract_frame)->add_slider(name, _min, _max, current, [callback](auto handle, int rt) {
        (*callback)((component_slider_handle_t)handle, rt);
    }, opts));
}

REFLECTION_API component_input_handle_t component_frame_add_input(component_frame_handle_t handle, const char* value, input_callback_t callback, input_options_t* options) {
    CHECK_HANDLE(handle, nullptr);

    reflection::component::input_options_t opts {};
    opts.submit_button_text = options->submit_button_text;

    return (component_input_handle_t)UNWRAP_COMPONENT(COMPONENT_CAST(handle, abstract_frame)->add_input(value, [callback](auto handle, const std::string& rt) {
        return std::string((*callback)((component_input_handle_t)handle, rt.data()));
    }, opts));
}

REFLECTION_API component_image_handle_t component_frame_add_image(component_frame_handle_t handle, const char* filepath, int width, int height, image_callback_t callback) {
    CHECK_HANDLE(handle, nullptr);

    return (component_image_handle_t)UNWRAP_COMPONENT(COMPONENT_CAST(handle, abstract_frame)->add_image(filepath, width, height, [callback](auto handle) {
        (*callback)((component_image_handle_t)handle);
    }));
}

REFLECTION_API component_folder_selector_handle_t component_frame_add_folder_selector(component_frame_handle_t handle, const char* name, const char* default_path, folder_selector_callback_t callback) {
    CHECK_HANDLE(handle, nullptr);

    return (component_folder_selector_handle_t)UNWRAP_COMPONENT(COMPONENT_CAST(handle, abstract_frame)->add_folder_selector(name, default_path, [callback](auto handle, std::string rt) {
        (*callback)((component_folder_selector_handle_t)handle, rt.data());
    }));
}

REFLECTION_API component_list_handle_t component_frame_add_list(component_frame_handle_t handle, const char* name, const char* active_items[], int active_items_size, list_callback_t callback) {
    CHECK_HANDLE(handle, nullptr);

    std::vector<std::string> vec_active_items(active_items, active_items + active_items_size);

    return (component_list_handle_t)UNWRAP_COMPONENT(COMPONENT_CAST(handle, abstract_frame)->add_list(name, vec_active_items, [callback](auto handle, int rt) {
        (*callback)((component_list_handle_t)handle, rt);
    }));
}

REFLECTION_API void component_button_set_disabled(component_button_handle_t handle, bool state) {
    CHECK_HANDLE(handle);
    COMPONENT_CAST(handle, abstract_button)->set_disabled(state);
}

REFLECTION_API bool component_checkbox_get_state(component_checkbox_handle_t handle) {
    CHECK_HANDLE(handle, false);
    return COMPONENT_CAST(handle, abstract_checkbox)->get_state();
}

REFLECTION_API unsafe_string_t* component_folder_selector_get_path(component_folder_selector_handle_t handle) {
    CHECK_HANDLE(handle, nullptr);

    auto str = COMPONENT_CAST(handle, abstract_folder_selector)->get_path();
    unsafe_string_t* result = new unsafe_string_t();

    result->string = new char[str.size() + 1];
    std::copy(str.begin(), str.end(), result->string);
    result->string[str.size()] = '\0';

    result->size = (int)(str.size() + 1ull);

    return result;
}

REFLECTION_API unsafe_string_t* component_input_get_value(component_input_handle_t handle) {
    CHECK_HANDLE(handle, nullptr);

    auto str = COMPONENT_CAST(handle, abstract_input)->get_value();
    unsafe_string_t* result = new unsafe_string_t();

    result->string = new char[str.size() + 1];
    std::copy(str.begin(), str.end(), result->string);
    result->string[str.size()] = '\0';

    result->size = (int)(str.size() + 1ull);

    return result;
}

REFLECTION_API void component_list_set_items(component_list_handle_t handle, const char* items[], int size) {
    CHECK_HANDLE(handle);

    std::vector<std::string> vec_items(items, items + size);
    COMPONENT_CAST(handle, abstract_list)->set_items(vec_items);
}

REFLECTION_API unsafe_int_array_t* component_selector_get_active_index(component_selector_handle_t handle) {
    CHECK_HANDLE(handle, nullptr);

    auto items = COMPONENT_CAST(handle, abstract_selector)->get_active_index();

    unsafe_int_array_t* arr = new unsafe_int_array_t();
    arr->size = (int)items.size();
    arr->array = new int[arr->size];
    std::copy(items.begin(), items.end(), arr->array);
    return arr;
}

REFLECTION_API int component_slider_get_value(component_slider_handle_t handle) {
    CHECK_HANDLE(handle, 0);
    return COMPONENT_CAST(handle, abstract_slider)->get_value();
}

REFLECTION_API void destroy_all_browsers() {
    for (auto& [ hwnd, browser ] : g_browser_windows) {
        browser->close();
        browser = nullptr;
    }
}

REFLECTION_API void destroy_browser(browser_handle_t* browser) {
    for (auto& [hwnd, _browser] : g_browser_windows) {
        if (_browser && _browser.get() == BROWSER_WINDOW_CAST(browser)) {
            _browser->close();
            _browser = nullptr;
            g_browser_windows.erase(hwnd);
            break;
        }
    }
}