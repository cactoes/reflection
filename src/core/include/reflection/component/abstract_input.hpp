#pragma once

#ifndef __REFLECTION_ABSTRACT_INPUT_HPP__
#define __REFLECTION_ABSTRACT_INPUT_HPP__

#include "abstract_component.hpp"

namespace reflection::component {
    struct input_options_t {
        // if no text given -> no submit_button
        std::string submit_button_text = "";
    };

    class abstract_input : public abstract_component {
    public:
        virtual ~abstract_input() = default;

        virtual std::string get_value() const = 0;
    
    protected:
        input_options_t m_options {};
    };

    typedef ru_function<std::string, abstract_input*, const std::string&> input_callback_t;
} // namespace reflection::component

#endif // __REFLECTION_ABSTRACT_INPUT_HPP__