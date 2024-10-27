#include <reflection/reflection.hpp>
#include <reflection/utils.hpp>

#include <core/components/frame.hpp>
#include <core/webview_browser_api.hpp>

#include <iostream>

reflection::browser_window::browser_window(HWND hwindow, reflection::component::frame_options_t frame_options) {    
    m_browser_api = std::make_unique<webview_browser_api>(hwindow);
    frame_options.max_size = true;
    frame_options.outline = false;
    m_frame = std::make_shared<component::frame>(m_browser_api.get(), "main", "", frame_options);

    m_browser_api->register_function("render_layout", [this](abstract_browser_api*, const std::vector<js_value>&) -> std::optional<std::any> {
        for (auto& event : m_events[event_t::E_ON_RENDER_START])
            event(this);

        m_browser_api->set_icon(ru_hash_base64(utils::load_resource(1).value_or(std::vector<uint8_t>{})));
        m_browser_api->set_title(utils::get_window_title(m_browser_api->get_window_handle()));
        if (m_color.size() != 0)
            m_browser_api->set_color(m_color);
        m_frame->render();
        return std::nullopt;
    });

    m_browser_api->register_function("on_render_finished", [this](abstract_browser_api*, const std::vector<js_value>&) {
        for (auto& event : m_events[event_t::E_ON_RENDER_FINISHED])
            event(this);

        return std::nullopt;
    });

    PostMessageA(m_browser_api->get_window_handle(), WM_HANDLE_WINDOW_CREATE, 0, 0);
}

void reflection::browser_window::start() {
    m_browser_api->start();
}

std::shared_ptr<reflection::component::abstract_frame> reflection::browser_window::get_frame() {
    return m_frame;
}

bool reflection::handle_window_message(HWND wnd, UINT msg, WPARAM, LPARAM) {
    const auto handle = reinterpret_cast<abstract_browser_api*>(GetWindowLongPtr(wnd, GWLP_USERDATA));

    if (!handle)
        return false;

    switch (msg) {
        case WM_SIZE:
            if (RECT bounds; GetClientRect(wnd, &bounds))
                handle->set_bounds(bounds);
            return true;
        case WM_HANDLE_BROWSER_EVENTS:
            handle->handle_events();
            return true;
        default:
            return false;
    }
}

bool reflection::browser_window::is_ready() const {
    return m_browser_api->is_ready();
}

void reflection::browser_window::register_event_handler(event_t event, const event_handler_t& callback) {
    m_events[event].push_back(callback);
}

void reflection::browser_window::set_icon(int resource_id) {
    if (m_browser_api->is_ready())
        m_browser_api->set_icon(ru_hash_base64(utils::load_resource(resource_id).value_or(std::vector<uint8_t>{})));
}

void reflection::browser_window::close() {
    if (m_browser_api)
        DestroyWindow(m_browser_api->get_window_handle());
}

void reflection::browser_window::set_color(const std::string& color) {
    m_color = color;

    if (m_browser_api->is_ready())
        m_browser_api->set_color(m_color);
}