#pragma once

#ifndef __FOLDER_SELECTOR_HPP__
#define __FOLDER_SELECTOR_HPP__

#include "../../reflection/component/abstract_folder_selector.hpp"
#include "../../reflection/utils.hpp"

namespace reflection::component {
    class folder_selector : public abstract_folder_selector {
    public:
        folder_selector(abstract_browser_api* browser, const std::string& target_id, const std::string& name, const std::string& default_path, const folder_selector_callback_t& callback)
            : m_callback(callback) {
            m_name = name;
            m_browser = browser;
            m_target_id = target_id;
            m_id = ru_random_uuid();
            m_path = default_path;
        }

        std::optional<std::any> on_change(abstract_browser_api*, const std::vector<js_value>& args) {
            m_path = args.at(0).as<std::string>();
            m_callback(this, m_path);
            return std::nullopt;
        }

        std::string get_path() const override {
            return m_path;
        }

        void render() override {
            m_browser->call_function("create_ui_component", { "folder_selector", m_target_id, m_id, m_name, m_path, COMPONENT_CALLER_ID(on_change) });
            m_browser->register_function(COMPONENT_CALLER(folder_selector, on_change));
        }

    private:
        std::string m_path;
        folder_selector_callback_t m_callback;
    };
} // namespace component

#endif // __FOLDER_SELECTOR_HPP__