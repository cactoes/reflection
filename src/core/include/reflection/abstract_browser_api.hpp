#pragma once

#ifndef __REFLECTION_ABSTRACT_BROWSER_API_HPP__
#define __REFLECTION_ABSTRACT_BROWSER_API_HPP__

#define WIN32_LEAN_AND_MEAN

#include <optional>
#include <any>
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <filesystem>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <variant>
#include <Windows.h>
#include "utils.hpp"

#define WM_HANDLE_BROWSER_EVENTS (WM_USER + 0x0001)
#define WM_HANDLE_WINDOW_CREATE (WM_USER + 0x0002)

// ~~ abstract browser class

template<typename Ty>
struct is_valid_type : std::disjunction<
    std::is_same<Ty, int>,
    std::is_same<Ty, float>,
    std::is_same<Ty, bool>,
    std::is_same<Ty, std::string>,
    std::is_same<Ty, void>,
    std::is_same<Ty, std::vector<std::any>>
> {};

template<typename Ty>
constexpr bool is_valid_type_v = is_valid_type<Ty>::value;

// using js_variant_base = std::variant<int, float, bool, std::string, const char*>;
// using js_variant = std::variant<js_variant_base, std::vector<js_variant_base>>;

class js_value {
public:
    js_value(const std::any& v) : m_value(v) {};

    template <typename Ty>
    std::enable_if_t<is_valid_type_v<Ty>, Ty>
    as() const {
        return std::any_cast<Ty>(m_value);
    }

    std::vector<js_value> as_array() const {
        return std::any_cast<std::vector<js_value>>(m_value);
    }

    template <typename Ty>
    std::vector<Ty> as_array() const {
        const std::vector<js_value> vec_raw = std::any_cast<std::vector<js_value>>(m_value);
        std::vector<Ty> vec_out {};
        vec_out.reserve(vec_raw.size());

        for (const auto& v : vec_raw)
            vec_out.push_back(v.as<Ty>());

        return vec_out;
    }

    template <typename Ty, typename = std::enable_if_t<is_valid_type_v<Ty>>>
    operator Ty() const {
        return as<Ty>();
    }

    const std::any& raw() const {
        return m_value;
    }

private:
    std::any m_value;
};

class abstract_browser_api;
typedef std::function<std::optional<std::any>(abstract_browser_api*, const std::vector<js_value>&)> browser_function_t;

struct browser_event_t {
    std::string name;
    std::vector<std::any> args;
};

struct browser_function_call_t {
    std::string function;
    std::vector<js_value> args;
};

class abstract_browser_api {
public:
    virtual ~abstract_browser_api() = default;

    // get the window handle ptr
    virtual HWND get_window_handle() = 0;

    // returns the path where any browser stores its cache files
    virtual const std::filesystem::path& get_cache_path() = 0;

    // for any window based events
    // must implement events:
    // - "we_move_window"
    // - "we_open_folder_selector"
    // - "we_minimize"
    // - "we_resize"
    // - "we_close"
    virtual std::optional<std::any> handle_window_events(abstract_browser_api*, const std::vector<js_value>&) = 0;

    virtual void start() = 0;

    virtual void set_title(const std::string& title) = 0;
    virtual void set_icon(const std::string& base64_data) = 0;
    virtual void set_color(const std::string& color) = 0;

    virtual void set_bounds(const RECT& bounds) const = 0;

    bool is_ready() const { return m_is_ready; }

    // handle call_function & other events
    void handle_events();

    // makes js able to call c++ functions
    void register_function(const std::string& name, browser_function_t func);
    
    template<typename _object>
    void register_function(const std::string& name, std::optional<std::any>(_object::*func)(abstract_browser_api*, const std::vector<js_value>&), _object* object) {
        register_function(name, [object, func](abstract_browser_api* api, const std::vector<js_value>& args) -> std::optional<std::any> { return (object->*func)(api, args); });
    }

    // makes c++ able to call js functions
    void call_function(const std::string& name, const std::vector<std::any>& args);

protected:
    virtual void execute_event(const browser_event_t& event) = 0;

protected:
    bool m_is_ready = false;
    reflection::utils::mt_bus<browser_event_t> m_event_handler = {};
    std::unordered_map<std::string, browser_function_t> m_functions = {};
    const std::thread::id m_thread_created_id = std::this_thread::get_id();
};

#endif // __REFLECTION_ABSTRACT_BROWSER_API_HPP__