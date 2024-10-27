#pragma once

#ifndef __SLIDER_HPP__
#define __SLIDER_HPP__

#include "../../reflection/component/abstract_slider.hpp"
#include "../../reflection/utils.hpp"

namespace reflection::component {
    class slider : public abstract_slider {
    public:
        slider(abstract_browser_api* browser, const std::string& target_id, const std::string& text, int min, int max, int current, const slider_callback_t& callback, const slider_options_t& options)
            : m_callback(callback) {
            m_name = text;
            m_browser = browser;
            m_target_id = target_id;
            m_id = ru_random_uuid();
            m_min = min;
            m_max = max;
            m_current = current;
            m_options = options;
        }

        std::optional<std::any> on_change(abstract_browser_api*, const std::vector<js_value>& args) {
            m_current = args.at(0).as<int>();
            m_callback(this, m_current);
            return std::nullopt;
        }

        int get_value() const override {
            return m_current;
        }

        void render() override {
            m_browser->call_function("create_ui_component", { "slider", m_target_id, m_id, m_name, m_min, m_max, m_current, COMPONENT_CALLER_ID(on_change), std::vector<std::any>{ m_options.show_ticks, m_options.tick_name_left, m_options.tick_name_right } });
            m_browser->register_function(COMPONENT_CALLER(slider, on_change));
        }

    private:
        int m_min;
        int m_max;
        int m_current;
        slider_callback_t m_callback;
        slider_options_t m_options;
    };
} // namespace component

#endif // __SLIDER_HPP__