#pragma once

#ifndef __REFLECTION_CORE_HPP__
#define __REFLECTION_CORE_HPP__

#include <unordered_map>

#include "component/abstract_frame.hpp"

namespace reflection {
    class browser_window;
    typedef ru_function<void, const browser_window*> event_handler_t;

    enum event_t {
        E_ON_RENDER_START = 0,
        E_ON_RENDER_FINISHED
    };

    class browser_window {
    public:
        browser_window(HWND hwindow, component::frame_options_t frame_options);

        void start();
        std::shared_ptr<component::abstract_frame> get_frame();
        bool is_ready() const;

        void register_event_handler(event_t event, const event_handler_t& callback);

        void set_icon(int resource_id);
        void set_color(const std::string& color);

        void close();

    private:
        std::unique_ptr<abstract_browser_api> m_browser_api;
        std::shared_ptr<component::abstract_frame> m_frame;
        std::string m_color;

        std::unordered_map<event_t, std::vector<event_handler_t>> m_events = {};
    };

    bool handle_window_message(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

    // LRESULT window_messsage_handler(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
} // namespace reflection

#endif // __REFLECTION_CORE_HPP__