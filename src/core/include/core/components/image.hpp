#pragma once

#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include "../../reflection/component/abstract_image.hpp"
#include "../../reflection/utils.hpp"

namespace reflection::component {
    class image : public abstract_image {
    public:
        image(abstract_browser_api* browser, const std::string& target_id, const std::string& path, int width, int height, const image_callback_t& callback)
            : m_callback(callback) {
            m_name = ru_hash_base64(utils::file_to_buffer(path).value_or(std::vector<uint8_t>{}));
            m_browser = browser;
            m_target_id = target_id;
            m_id = ru_random_uuid();
            m_width = width;
            m_height = height;
        }

        std::optional<std::any> on_click(abstract_browser_api*, const std::vector<js_value>&) {
            m_callback(this);
            return std::nullopt;
        }

        void render() override {
            m_browser->call_function("create_ui_component", { "image", m_target_id, m_id, m_name, m_width, m_height, COMPONENT_CALLER_ID(on_click) });
            m_browser->register_function(COMPONENT_CALLER(image, on_click));
        }

    private:
        int m_width;
        int m_height;
        image_callback_t m_callback;
    };
} // namespace component

#endif // __IMAGE_HPP__