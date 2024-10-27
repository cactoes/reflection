#include <iostream>
#include <reflection.hpp>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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

HWND create_window(const std::string& window_name, HINSTANCE instance = nullptr, const std::string& window_class_name = "wnd_cls") {
    const HICON icon = LoadIconA(instance, MAKEINTRESOURCE(1));

    WNDCLASSEX window_class{};
    window_class.cbSize = sizeof(WNDCLASSEX);
    window_class.style = (CS_HREDRAW | CS_VREDRAW);
    window_class.lpfnWndProc = &WindowProc;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = instance;
    window_class.hCursor = LoadCursorA(nullptr, IDC_ARROW);
    window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    window_class.lpszMenuName = nullptr;
    window_class.lpszClassName = window_class_name.c_str();
    window_class.hIcon = icon;
    window_class.hIconSm = icon;

    RegisterClassExA(&window_class);
    
    const DWORD window_style_flags = WS_POPUP | WS_VISIBLE;

    const int window_pos_x = (GetSystemMetrics(SM_CXSCREEN) - 700) / 2;
    const int window_pos_y = (GetSystemMetrics(SM_CYSCREEN) - 600) / 2;

    const HWND window_handle = CreateWindowExA(
        0,
        window_class_name.c_str(),
        window_name.c_str(),
        window_style_flags,
        window_pos_x, window_pos_y,
        700, 600,
        NULL, NULL,
        instance,
        NULL);

    ShowWindow(window_handle, SW_SHOW);
    UpdateWindow(window_handle);

    return window_handle;
}

// int main() {
int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    auto options = reflection::component::frame_options_t {};
    options.is_tab_list = true;
    reflection::browser_window window(create_window("Test window"), options);

    window.register_event_handler(reflection::E_ON_RENDER_FINISHED, [](auto) {
        std::cout << "render is finished\n";
    });

    auto main_frame = window.get_frame();

    auto tab_1 = main_frame->add_frame("Tab 1");
    main_frame->add_frame("Tab 2");

    auto sub_frame1 = tab_1->add_frame("Sub frame 1", { .max_size = true });
    sub_frame1->add_checkbox("Checkbox", false, [](auto, bool state) { std::cout << "state: " << (state ? "checked" : "unchecked") << "\n"; });
    sub_frame1->add_checkbox("Checkbox", true, [](auto, bool state) { std::cout << "state: " << (state ? "checked" : "unchecked") << "\n"; });
    sub_frame1->add_folder_selector("Folder Selector", "C:\\", [](auto, std::string path){ std::cout << "path: " << path << "\n"; });

    reflection::component::frame_options_t opt2{};
    opt2.max_size = true;
    opt2.align = reflection::component::fa_center;
    sub_frame1->add_frame("test", opt2)
        ->add_image("disc_icon_2.ico", 30, 30, [](auto) { std::cout << "clicked\n"; });

    tab_1->add_label("Label");
    tab_1->add_selector("Selector", std::vector<std::string>{ "Option 1", "Option 2", "Option 3" }, std::vector<int>{ 0 }, [](auto, auto indexs) { for (const auto& v : indexs) std::cout << "selected: " << v << "\n"; });
    tab_1->add_selector("Selector", std::vector<std::string>{ "Option 1", "Option 2", "Option 3" }, std::vector<int>{ }, [](auto, auto indexs) { for (const auto& v : indexs) std::cout << "selected: " << v << "\n"; }, { .is_multi = true });
    tab_1->add_slider("Slider", 0, 10, 0, [&](auto, int v) { std::cout << "value: " << v << "\n"; }, { .tick_name_left = "Min", .tick_name_right = "Max" });
    
    auto ff1 = tab_1->add_frame("", { .layout = reflection::component::fl_vertical });
    ff1->add_button("Button", [&](auto) { std::cout << "clicked\n"; }, { .full_width = true });
    ff1->add_button("Button", [&](auto) { std::cout << "clicked\n"; }, { .disabled = true, .full_width = true });
    
    auto ff2 = tab_1->add_frame("", { .outline = false, .layout = reflection::component::fl_horizontal});
    ff2->add_button("Button", [&](auto) { std::cout << "clicked\n"; }, { .full_width = true });
    ff2->add_button("Button", [&](auto) { std::cout << "clicked\n"; }, { .disabled = true, .full_width = true });
    
    tab_1->add_input("default value", [](auto, const std::string& v) { std::cout << "input: " << v << "\n"; return v; });
    tab_1->add_input("default value", [](auto, const std::string& v) { std::cout << "input: " << v << "\n"; return v; }, { .submit_button_text = "Submit" });
    tab_1->add_list("List", std::vector<std::string>{ "item1", "item2", "item3", "item4", "item5", "item6", "item7", "item8" }, [](auto, int v) { std::cout << "clicked item: " << v << "\n"; });

    window.start();

    MSG msg{};
    while (GetMessageA(&msg, nullptr, 0, 0) != 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return 0;
}