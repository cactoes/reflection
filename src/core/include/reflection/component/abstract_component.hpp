#pragma once

#ifndef __REFLECTION_ABSTRACT_COMPONENT_HPP__
#define __REFLECTION_ABSTRACT_COMPONENT_HPP__

#include <string>

#include "../abstract_browser_api.hpp"

#define COMPONENT_CALLER_ID(callback) \
    m_id + #callback

#define COMPONENT_CALLER(_class, callback) \
    COMPONENT_CALLER_ID(callback), &_class##::##callback, this

namespace reflection::component {
    class abstract_component {
    public:
        virtual ~abstract_component() = default;

        virtual void render() = 0;

        // will not update ui (use before render)
        void set_target(const std::string& target_id) { m_target_id = target_id; }
        // will not update ui (use before render)
        void set_id(const std::string& id) { m_id = id; }

        void set_name(const std::string& name) {
            m_name = name;
            render();
        }

        const std::string& get_id() const {
            return m_id;
        }

    protected:
        std::string m_name {};
        std::string m_target_id {};
        std::string m_id {};
        abstract_browser_api* m_browser = nullptr;
    };
} // namespace reflection::component

#endif // __REFLECTION_ABSTRACT_COMPONENT_HPP__