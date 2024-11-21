#pragma once

#ifndef __CANVAS_HPP__
#define __CANVAS_HPP__

#include "../../reflection/component/abstract_canvas.hpp"
#include "../../reflection/utils.hpp"

namespace reflection::component {
    class canvas : public abstract_canvas {
    public:
        canvas(abstract_browser_api* browser, const std::string& target_id, const std::string& text, int width, int height, std::vector<uint32_t>* buffer) {
            m_name = text;
            m_browser = browser;
            m_target_id = target_id;
            m_id = ru_random_uuid();
            m_width = width;
            m_height = height;
            m_buffer_ptr = buffer;
        }

        void update_buffer() override {
            m_browser->call_function("update_ui_component_data", { "canvas", m_id, *m_buffer_ptr, m_width, m_height });
        }

        std::optional<std::any> update_buffer(abstract_browser_api*, const std::vector<js_value>&) {
            m_browser->call_function("update_ui_component_data", { "canvas", m_id, *m_buffer_ptr, m_width, m_height });
            return std::nullopt;
        }

        void render() override {
            m_browser->call_function("create_ui_component", { "canvas", m_target_id, m_id, m_name, m_width, m_height, COMPONENT_CALLER_ID(update_buffer) });
            m_browser->register_function(COMPONENT_CALLER(canvas, update_buffer));
        }

    private:
        int m_width;
        int m_height;
        std::vector<uint32_t>* m_buffer_ptr;
    };
} // namespace component

#endif // __CANVAS_HPP__