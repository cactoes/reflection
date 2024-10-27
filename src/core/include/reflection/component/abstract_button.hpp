#pragma once

#ifndef __REFLECTION_ABSTRACT_BUTTON_HPP__
#define __REFLECTION_ABSTRACT_BUTTON_HPP__

#include "abstract_component.hpp"

namespace reflection::component {
    struct button_options_t {
        bool disabled = false;
        bool full_width = false;
    };

    class abstract_button : public abstract_component {
    public:
        virtual ~abstract_button() = default;

        virtual void set_disabled(bool state) = 0;

    protected:
        button_options_t m_options {};
    };

    typedef ru_function<void, const abstract_button*> button_callback_t;
} // namespace reflection::component

#endif // __REFLECTION_ABSTRACT_BUTTON_HPP__