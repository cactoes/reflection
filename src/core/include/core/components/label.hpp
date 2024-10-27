#pragma once

#ifndef __COMPONENT_LABEL_HPP__
#define __COMPONENT_LABEL_HPP__

#include "../../reflection/component/abstract_label.hpp"
#include "../../reflection/utils.hpp"

namespace reflection::component {
    class label : public abstract_label {
    public:
        label(abstract_browser_api* browser, const std::string& target_id, const std::string& text) {
            m_name = text;
            m_browser = browser;
            m_target_id = target_id;
            m_id = ru_random_uuid();
        }

        void render() override {
            m_browser->call_function("create_ui_component", { "label", m_target_id, m_id, m_name });
        }
    };
} // namespace component

#endif // __COMPONENT_LABEL_HPP__