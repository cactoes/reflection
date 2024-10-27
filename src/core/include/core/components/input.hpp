#pragma once

#ifndef __INPUT_HPP__
#define __INPUT_HPP__


#include "../../reflection/component/abstract_input.hpp"
#include "../../reflection/utils.hpp"

namespace reflection::component {
    class input : public abstract_input {
    public:
        input(abstract_browser_api* browser, const std::string& target_id, const std::string& value, const input_callback_t& callback, const input_options_t& options)
            : m_callback(callback) {
            m_name = value;
            m_browser = browser;
            m_target_id = target_id;
            m_id = ru_random_uuid();
            m_options = options;
        }

        std::optional<std::any> on_change(abstract_browser_api*, const std::vector<js_value>& args) {
            const auto default_value = args.at(0).as<std::string>();
            m_name = m_callback(this, default_value).value_or(default_value);
            return std::any(m_name);
        }


        std::string get_value() const override {
            return m_name;
        }

        void render() override {
            m_browser->call_function("create_ui_component", { "input", m_target_id, m_id, m_name, COMPONENT_CALLER_ID(on_change), std::vector<std::any>{ m_options.submit_button_text } });
            m_browser->register_function(COMPONENT_CALLER(input, on_change));
        }

    private:
        input_callback_t m_callback;
    };
} // namespace component

#endif // __INPUT_HPP__