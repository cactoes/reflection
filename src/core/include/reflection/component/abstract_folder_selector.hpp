#pragma once

#ifndef __REFLECTION_ABSTRACT_FOLDER_SELECTOR_HPP__
#define __REFLECTION_ABSTRACT_FOLDER_SELECTOR_HPP__

#include "abstract_component.hpp"

namespace reflection::component {
    class abstract_folder_selector : public abstract_component {
    public:
        virtual ~abstract_folder_selector() = default;

        virtual std::string get_path() const = 0;
    };

    typedef ru_function<void, const abstract_folder_selector*, std::string> folder_selector_callback_t;
} // namespace reflection::component


#endif // __REFLECTION_ABSTRACT_FOLDER_SELECTOR_HPP__