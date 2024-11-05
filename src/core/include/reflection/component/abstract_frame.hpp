#pragma once

#ifndef __REFLECTION_ABSTRACT_FRAME_HPP__
#define __REFLECTION_ABSTRACT_FRAME_HPP__

#include "abstract_component.hpp"
#include "abstract_label.hpp"
#include "abstract_checkbox.hpp"
#include "abstract_button.hpp"
#include "abstract_selector.hpp"
#include "abstract_slider.hpp"
#include "abstract_input.hpp"
#include "abstract_image.hpp"
#include "abstract_folder_selector.hpp"
#include "abstract_list.hpp"

namespace reflection::component {
    enum frame_layout_t {
        fl_horizontal = 0,
        fl_vertical
    };

    enum frame_align_t {
        fa_none = 0,
        fa_horizontal,
        fa_vertical,
        fa_center,
    };

    struct frame_options_t {
        bool is_tab_list = false;
        bool outline = true;
        frame_layout_t layout = frame_layout_t::fl_vertical;
        bool max_size = false;
        frame_align_t align = frame_align_t::fa_none;
        bool overflow = false;
        bool border = true;
    };

    class abstract_frame : public abstract_component {
    public:
        virtual ~abstract_frame() = default;

        virtual std::shared_ptr<abstract_frame> add_frame(const std::string& name, const frame_options_t& options = {}) = 0;
        virtual std::optional<std::shared_ptr<abstract_label>> add_label(const std::string& text) = 0;
        virtual std::optional<std::shared_ptr<abstract_button>> add_button(const std::string& name, const button_callback_t& callback = {}, const button_options_t& options = {}) = 0;
        virtual std::optional<std::shared_ptr<abstract_checkbox>> add_checkbox(const std::string& name, bool state, const checkbox_callback_t& callback = {}) = 0;
        virtual std::optional<std::shared_ptr<abstract_selector>> add_selector(const std::string& name, const std::vector<std::string>& list_options, const std::vector<int>& index, const selector_callback_t& callback = {}, const selector_options_t& options = {}) = 0;
        virtual std::optional<std::shared_ptr<abstract_slider>> add_slider(const std::string& name, int min, int max, int current, const slider_callback_t& callback = {}, const slider_options_t& options = {}) = 0;
        virtual std::optional<std::shared_ptr<abstract_input>> add_input(const std::string& value, const input_callback_t& callback = {}, const input_options_t& options = {}) = 0;
        virtual std::optional<std::shared_ptr<abstract_image>> add_image(const std::string& filepath, int width, int height, const image_callback_t& callback = {}) = 0;
        virtual std::optional<std::shared_ptr<abstract_folder_selector>> add_folder_selector(const std::string& name, const std::string& default_path, const folder_selector_callback_t& callback = {}) = 0;
        virtual std::optional<std::shared_ptr<abstract_list>> add_list(const std::string& name, const std::vector<std::string>& active_items, const list_callback_t& callback = {}) = 0;

        template <typename Ty>
        std::enable_if_t<std::is_base_of_v<abstract_component, Ty>, std::optional<std::shared_ptr<Ty>>>
        get_component(const std::string& id) {
            for (auto& component : m_components)
                if (component->get_id() == id)
                    return std::static_pointer_cast<Ty>(component);

            return std::nullopt;
        }

        std::shared_ptr<abstract_component> add_component(std::shared_ptr<abstract_component> component) {
            component->set_target(m_id);
            m_components.push_back(component);
            return component;
        }

        template <typename _abstract_component_type, typename _component_type, typename... _arguments>
        std::enable_if_t<std::is_constructible_v<_component_type, _arguments...>, std::optional<std::shared_ptr<_abstract_component_type>>> add_component(_arguments&&... args) {
            if (m_options.is_tab_list)
                return std::nullopt;

            return std::dynamic_pointer_cast<_abstract_component_type>(add_component(std::make_shared<_component_type>(std::forward<_arguments>(args)...)));
        }

    protected:
        frame_options_t m_options {};
        std::vector<std::shared_ptr<abstract_component>> m_components {};
    };
} // namespace reflection::component

#endif // __REFLECTION_ABSTRACT_FRAME_HPP__