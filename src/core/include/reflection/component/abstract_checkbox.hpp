#pragma once

#ifndef __REFLECTION_ABSTRACT_CHECKBOX_HPP__
#define __REFLECTION_ABSTRACT_CHECKBOX_HPP__

#include "abstract_component.hpp"

namespace reflection::component {
    class abstract_checkbox : public abstract_component {
    public:
        virtual ~abstract_checkbox() = default;

        virtual bool get_state() const = 0;
    };

    typedef ru_function<void, abstract_checkbox*, bool> checkbox_callback_t;
} // namespace reflection::component

#endif // __REFLECTION_ABSTRACT_CHECKBOX_HPP__