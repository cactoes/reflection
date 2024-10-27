#pragma once

#ifndef __SELECTOR_HPP__
#define __SELECTOR_HPP__

#include "../../reflection/component/abstract_selector.hpp"
#include "../../reflection/utils.hpp"

namespace reflection::component {
    class selector : public abstract_selector {
    public:
        selector(abstract_browser_api* browser, const std::string& target_id, const std::string& text, const std::vector<std::string>& list_options, const std::vector<int>& index, const selector_callback_t& callback, const selector_options_t& options)
            : m_callback(callback) {
            m_name = text;
            m_browser = browser;
            m_target_id = target_id;
            m_id = ru_random_uuid();
            m_list_options = list_options;
            m_options = options;

            if (!options.is_multi && index.size() > 1) {
                m_index = std::vector<int>(index.begin(), index.begin() + 1);
            } else {
                m_index = index;
            }
        }

        std::optional<std::any> on_click(abstract_browser_api*, const std::vector<js_value>& args) {
            m_index = args.at(0).as_array<int>();
            m_callback(this, m_index);
            return std::nullopt;
        }

        std::vector<int> get_active_index() const {
            return m_index;
        }

        void render() override {
            m_browser->call_function("create_ui_component", { "selector", m_target_id, m_id, m_name, m_list_options, m_index, COMPONENT_CALLER_ID(on_click), std::vector<std::any>{ m_options.is_multi } });
            m_browser->register_function(COMPONENT_CALLER(selector, on_click));
        }
    
    private:
        std::vector<std::string> m_list_options;
        std::vector<int> m_index;
        selector_callback_t m_callback;
    };
} // namespace component

#endif // __SELECTOR_HPP__