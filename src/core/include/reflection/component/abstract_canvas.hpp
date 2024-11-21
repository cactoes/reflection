#pragma once

#ifndef __REFLECTION_ABSTRACT_CANVAS_HPP__
#define __REFLECTION_ABSTRACT_CANVAS_HPP__

#include "abstract_component.hpp"

namespace reflection::component {
    class abstract_canvas : public abstract_component {
    public:
        virtual ~abstract_canvas() = default;

        virtual void update_buffer() = 0;
    };
} // namespace reflection::component

#endif // __REFLECTION_ABSTRACT_CANVAS_HPP__