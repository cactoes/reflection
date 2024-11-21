#pragma once

#ifndef __FRAME_HPP__
#define __FRAME_HPP__

#include "label.hpp"
#include "checkbox.hpp"
#include "button.hpp"
#include "selector.hpp"
#include "slider.hpp"
#include "input.hpp"
#include "image.hpp"
#include "folder_selector.hpp"
#include "list.hpp"
#include "canvas.hpp"

#include "../../reflection/component/abstract_frame.hpp"
#include "../../reflection/utils.hpp"

namespace reflection::component {
    class frame : public abstract_frame {
    public:
        frame(abstract_browser_api* browser, const std::string& target_id, const std::string& name, const frame_options_t& options) {
            m_name = name;
            m_browser = browser;
            m_target_id = target_id;
            m_options = options;
            m_id = ru_random_uuid();
        }

        std::shared_ptr<abstract_frame> add_frame(const std::string& name, const frame_options_t& options) override {
            return std::dynamic_pointer_cast<abstract_frame>(add_component(std::make_shared<frame>(m_browser, m_id, name, options)));
        }

        std::optional<std::shared_ptr<abstract_label>> add_label(const std::string& text) override {
            return add_component<abstract_label, label>(m_browser, m_id, text);
        }

        std::optional<std::shared_ptr<abstract_button>> add_button(const std::string& name, const button_callback_t& callback, const button_options_t& options) override {
            return add_component<abstract_button, button>(m_browser, m_id, name, callback, options);
        }

        std::optional<std::shared_ptr<abstract_checkbox>> add_checkbox(const std::string& name, bool state, const checkbox_callback_t& callback) override {
            return add_component<abstract_checkbox, checkbox>(m_browser, m_id, name, state, callback);
        }

        std::optional<std::shared_ptr<abstract_selector>> add_selector(const std::string& name, const std::vector<std::string>& list_options, const std::vector<int>& index, const selector_callback_t& callback, const selector_options_t& options) override {
            return add_component<abstract_selector, selector>(m_browser, m_id, name, list_options, index, callback, options);
        }

        std::optional<std::shared_ptr<abstract_slider>> add_slider(const std::string& name, int min, int max, int current, const slider_callback_t& callback, const slider_options_t& options) override {
            return add_component<abstract_slider, slider>(m_browser, m_id, name, min, max, current, callback, options);
        }

        std::optional<std::shared_ptr<abstract_input>> add_input(const std::string& value, const input_callback_t& callback, const input_options_t& options = {}) override {
            return add_component<abstract_input, input>(m_browser, m_id, value, callback, options);
        }

        std::optional<std::shared_ptr<abstract_image>> add_image(const std::string& filepath, int width, int height, const image_callback_t& callback) override {
            return add_component<abstract_image, image>(m_browser, m_id, filepath, width, height, callback);
        }

        std::optional<std::shared_ptr<abstract_folder_selector>> add_folder_selector(const std::string& name, const std::string& default_path, const folder_selector_callback_t& callback) override {
            return add_component<abstract_folder_selector, folder_selector>(m_browser, m_id, name, default_path, callback);
        }

        std::optional<std::shared_ptr<abstract_list>> add_list(const std::string& name, const std::vector<std::string>& active_items, const list_callback_t& callback) override {
            return add_component<abstract_list, list>(m_browser, m_id, name, active_items, callback);
        }

        std::optional<std::shared_ptr<abstract_canvas>> add_canvas(const std::string& name, int width, int height, std::vector<uint32_t>* buffer) override {
            return add_component<abstract_canvas, canvas>(m_browser, m_id, name, width, height, buffer);
        }

        void render() override {
            m_browser->call_function("create_ui_component", { "frame", m_target_id, m_id, m_name, std::vector<std::any>{ m_options.is_tab_list, m_options.outline, (int)m_options.layout, m_options.max_size, (int)m_options.align, m_options.overflow, m_options.border } });
            for (const auto& component : m_components)
                component->render();
        }
    };
} // namespace component

#endif // __FRAME_HPP__