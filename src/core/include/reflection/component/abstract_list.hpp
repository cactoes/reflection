#pragma once

#ifndef __REFLECTION_ABSTRACT_LIST_HPP__
#define __REFLECTION_ABSTRACT_LIST_HPP__

#include "abstract_component.hpp"

namespace reflection::component {
    class abstract_list : public abstract_component {
    public:
        virtual ~abstract_list() = default;

        virtual void set_items(const std::vector<std::string>&) = 0;
    };

    typedef ru_function<void, abstract_list*, int> list_callback_t;
} // namespace reflection::component

#endif // __REFLECTION_ABSTRACT_LIST_HPP__