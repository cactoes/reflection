#pragma once

#ifndef __REFLECTION_ABSTRACT_SELECTOR_HPP__
#define __REFLECTION_ABSTRACT_SELECTOR_HPP__

#include "abstract_component.hpp"

namespace reflection::component {
    struct selector_options_t {
        bool is_multi = false;
    };

    class abstract_selector : public abstract_component {
    public:
        virtual ~abstract_selector() = default;

        virtual std::vector<int> get_active_index() const = 0;

    protected:
        selector_options_t m_options {};
    };

    typedef ru_function<void, const abstract_selector*, std::vector<int>> selector_callback_t;
} // namespace reflection::component

#endif // __REFLECTION_ABSTRACT_SELECTOR_HPP__