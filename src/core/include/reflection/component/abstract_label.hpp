#pragma once

#ifndef __REFLECTION_ABSTRACT_LABEL_HPP__
#define __REFLECTION_ABSTRACT_LABEL_HPP__

#include "abstract_component.hpp"

namespace reflection::component {
    class abstract_label : public abstract_component {
    public:
        virtual ~abstract_label() = default;
    };
} // namespace reflection::component

#endif // __REFLECTION_ABSTRACT_LABEL_HPP__