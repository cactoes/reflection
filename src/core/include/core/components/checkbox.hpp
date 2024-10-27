#pragma once

#ifndef __CHECKBOX_HPP__
#define __CHECKBOX_HPP__

#include "../../reflection/component/abstract_checkbox.hpp"
#include "../../reflection/utils.hpp"

namespace reflection::component {
    class checkbox : public abstract_checkbox {
    public:
        checkbox(abstract_browser_api* browser, const std::string& target_id, const std::string& text, bool state, const checkbox_callback_t& callback)
            : m_callback(callback) {
            m_name = text;
            m_browser = browser;
            m_target_id = target_id;
            m_id = ru_random_uuid();
            m_state = state;
        }

        std::optional<std::any> on_click(abstract_browser_api*, const std::vector<js_value>& args) {
            m_state = args.at(0).as<bool>();
            m_callback(this, m_state);
            return std::nullopt;
        }

        bool get_state() const override {
            return m_state;
        }

        void render() override {
            m_browser->call_function("create_ui_component", { "checkbox", m_target_id, m_id, m_name, m_state, COMPONENT_CALLER_ID(on_click)});
            m_browser->register_function(COMPONENT_CALLER(checkbox, on_click));
        }

    private:
        bool m_state;
        checkbox_callback_t m_callback;
    };
} // namespace component

#endif // __CHECKBOX_HPP__