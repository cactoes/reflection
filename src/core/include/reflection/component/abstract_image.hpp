#pragma once

#ifndef __REFLECTION_ABSTRACT_IMAGE_HPP__
#define __REFLECTION_ABSTRACT_IMAGE_HPP__

#include "abstract_component.hpp"

namespace reflection::component {
    class abstract_image : public abstract_component {
    public:
        virtual ~abstract_image() = default;
    };

    typedef ru_function<void, abstract_image*> image_callback_t;
} // namespace reflection::component

#endif // __REFLECTION_ABSTRACT_IMAGE_HPP__