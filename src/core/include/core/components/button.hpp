#pragma once

#ifndef __BUTTON_HPP__
#define __BUTTON_HPP__

#include "../../reflection/component/abstract_button.hpp"
#include "../../reflection/utils.hpp"

namespace reflection::component {
    class button : public abstract_button {
    public:
        button(abstract_browser_api* browser, const std::string& target_id, const std::string& text, const button_callback_t& callback, const button_options_t& options)
            : m_callback(callback) {
            m_name = text;
            m_browser = browser;
            m_target_id = target_id;
            m_id = ru_random_uuid();
            m_options = options;
        }

        void set_disabled(bool state) override {
            m_options.disabled = state;
            render();
        }

        std::optional<std::any> on_click(abstract_browser_api*, const std::vector<js_value>&) {
            m_callback(this);
            return std::nullopt;
        }

        void render() override {
            m_browser->call_function("create_ui_component", { "button", m_target_id, m_id, m_name, COMPONENT_CALLER_ID(on_click), std::vector<std::any>{ m_options.disabled, m_options.full_width } });
            m_browser->register_function(COMPONENT_CALLER(button, on_click));
        }

    private:
        button_callback_t m_callback;
    };
} // namespace component

#endif // __BUTTON_HPP__