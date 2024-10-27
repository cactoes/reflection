#pragma once

#ifndef __REFLECTION_ABSTRACT_SLIDER_HPP__
#define __REFLECTION_ABSTRACT_SLIDER_HPP__

#include "abstract_component.hpp"

namespace reflection::component {
    struct slider_options_t {
        bool show_ticks = true;
        std::string tick_name_left = "Low";
        std::string tick_name_right = "High";
    };

    class abstract_slider : public abstract_component {
    public:
        virtual ~abstract_slider() = default;

        virtual int get_value() const = 0;
    };

    typedef ru_function<void, const abstract_slider*, int> slider_callback_t;
} // namespace reflection::component

#endif // __REFLECTION_ABSTRACT_SLIDER_HPP__