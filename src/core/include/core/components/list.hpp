#pragma once

#ifndef __LIST_HPP__
#define __LIST_HPP__

#include "../../reflection/component/abstract_list.hpp"
#include "../../reflection/utils.hpp"

namespace reflection::component {
    class list : public abstract_list {
    public:
        list(abstract_browser_api* browser, const std::string& target_id, const std::string& name, const std::vector<std::string>& active_items, const list_callback_t& callback)
            : m_callback(callback) {
            m_name = name;
            m_browser = browser;
            m_target_id = target_id;
            m_id = ru_random_uuid();
            m_active_items = active_items;
        }

        std::optional<std::any> on_click_item(abstract_browser_api*, const std::vector<js_value>& args) {
            m_callback(this, args.at(0).as<int>());
            return std::nullopt;
        }

        void set_items(const std::vector<std::string>& items) override {
            m_active_items = items;
            render();
        }

        void render() override {
            m_browser->call_function("create_ui_component", { "list", m_target_id, m_id, m_name, m_active_items, COMPONENT_CALLER_ID(on_click_item) });
            m_browser->register_function(COMPONENT_CALLER(list, on_click_item));
        }
    
    private:
        std::vector<std::string> m_active_items;
        list_callback_t m_callback;
    };
} // namespace component

#endif // __LIST_HPP__