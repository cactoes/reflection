#pragma once

#ifndef __WEBVIEW_BROWSER_API_HPP__
#define __WEBVIEW_BROWSER_API_HPP__

#include <wil/com.h>
#include <wrl.h>
#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>
#include <nlohmann/json.hpp>
#include <shlobj.h>

#include "../reflection/abstract_browser_api.hpp"
#include "../reflection/utils.hpp"

int browse_callback_proc(HWND hwnd, UINT uMsg, LPARAM, LPARAM lpData);

class browser_interface {
public:
    std::string create_function_call(const browser_event_t& event) {
        std::string script = "__core_call_js__(\"" + event.name + "\",";

        for (const auto& arg : event.args)
            script += reflection::utils::replace(map_argument(arg), "\\", "\\\\") + ",";

        script[script.size() - 1] = ')';

        return script;
    }

    std::vector<js_value> parse_argument(const nlohmann::json& arg) {
        std::vector<js_value> out = {};
        switch (arg.type()) {
            case nlohmann::detail::value_t::string:
                out.push_back(js_value(arg.get<std::string>()));
                break;
            case nlohmann::detail::value_t::number_unsigned:
            case nlohmann::detail::value_t::number_integer:
                out.push_back(js_value(arg.get<int>()));
                break;
            case nlohmann::detail::value_t::boolean:
                out.push_back(js_value(arg.get<bool>()));
                break;
            case nlohmann::detail::value_t::number_float:
                out.push_back(js_value(arg.get<float>()));
                break;
            case nlohmann::detail::value_t::array: {
                std::vector<js_value> _out = {};
                for (const auto& i : arg.get<nlohmann::json::array_t>())
                    if (auto r = parse_argument(i); !r.empty())
                        _out.push_back(js_value(r.at(0)));
                out.push_back(js_value(_out));
                break;
            }
            default:
                break;
        }
        return out;
    }

    browser_function_call_t parse_message(const nlohmann::json& message) {
        const auto fn_name = message.at("__name__").get<std::string>();
        const auto fn_args = message.at("__args__").get<nlohmann::json::array_t>();
        std::vector<js_value> args = parse_argument(fn_args);
        return { fn_name, args.at(0).as_array() };
    }

    std::string create_result(const std::string& fn_name, std::any result) {
        return "__core_result_handler__(\"" + fn_name + "\"," + reflection::utils::replace(map_argument(result), "\\", "\\\\") + ");";
    }

private:
    template <typename Ty>
    std::string typed_to_string(const Ty& value) {
        if constexpr (std::is_same_v<Ty, int> || std::is_same_v<Ty, float> || std::is_same_v<Ty, bool> || std::is_same_v<Ty, uint8_t>)
            return std::to_string(value);
        else if constexpr (std::is_same_v<Ty, char*> || std::is_same_v<Ty, const char*>)
            return "\"" + std::string(value) + "\"";
        else if constexpr (std::is_same_v<Ty, std::string>)
            return "\"" + value + "\"";
        else
            return "undefined";
    }

    template <typename Ty>
    std::string typed_to_string(const std::vector<Ty>& vec) {
        std::ostringstream oss;
        for (size_t i = 0; i < vec.size(); ++i)
            oss << typed_to_string(vec.at(i)) << ", ";

        return "[" + oss.str() + "]";
    }

    std::string typed_to_string(const std::vector<std::any>& vec) {
        std::ostringstream oss;
        for (size_t i = 0; i < vec.size(); ++i)
            oss << map_argument(vec.at(i)) << ", ";

        return "[" + oss.str() + "]";
    }

    std::string map_argument(std::any argument) {
        const auto& argument_type = argument.type();

        if (argument_type == typeid(uint8_t))                   return typed_to_string(std::any_cast<uint8_t>(argument));
        if (argument_type == typeid(int))                       return typed_to_string(std::any_cast<int>(argument));
        if (argument_type == typeid(float))                     return typed_to_string(std::any_cast<float>(argument));
        if (argument_type == typeid(bool))                      return typed_to_string(std::any_cast<bool>(argument));
        if (argument_type == typeid(char*))                     return typed_to_string(std::any_cast<char*>(argument));
        if (argument_type == typeid(const char*))               return typed_to_string(std::any_cast<const char*>(argument));
        if (argument_type == typeid(std::string))               return typed_to_string(std::any_cast<std::string>(argument));

        if (argument_type == typeid(std::vector<uint8_t>))      return typed_to_string(std::any_cast<std::vector<uint8_t>>(argument));
        if (argument_type == typeid(std::vector<int>))          return typed_to_string(std::any_cast<std::vector<int>>(argument));
        if (argument_type == typeid(std::vector<float>))        return typed_to_string(std::any_cast<std::vector<float>>(argument));
        if (argument_type == typeid(std::vector<bool>))         return typed_to_string(std::any_cast<std::vector<bool>>(argument));
        if (argument_type == typeid(std::vector<char*>))        return typed_to_string(std::any_cast<std::vector<char*>>(argument));
        if (argument_type == typeid(std::vector<const char*>))  return typed_to_string(std::any_cast<std::vector<const char*>>(argument));
        if (argument_type == typeid(std::vector<std::string>))  return typed_to_string(std::any_cast<std::vector<std::string>>(argument));
        if (argument_type == typeid(std::vector<std::any>))     return typed_to_string(std::any_cast<std::vector<std::any>>(argument));

        return "undefined";
    }
};

class webview_browser_api : public abstract_browser_api {
public:
    webview_browser_api(HWND window);

    // int64_t handle_window_message(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
    HWND get_window_handle() override;
    const std::filesystem::path& get_cache_path() override;
    std::optional<std::any> handle_window_events(abstract_browser_api*, const std::vector<js_value>&) override;
    void start() override;

    void set_title(const std::string& title) override;
    void set_icon(const std::string& base64_data) override;
    void set_color(const std::string& color) override;
    void set_bounds(const RECT& bounds) const override;

private:
    void execute_event(const browser_event_t& event) override;

private:
    HWND m_window;

    std::filesystem::path m_cache_path;
    
    browser_interface m_browser_interface = {};

    wil::com_ptr<ICoreWebView2Controller> m_webview_controller = nullptr;
    wil::com_ptr<ICoreWebView2> m_webview2 = nullptr;
    wil::com_ptr<ICoreWebView2_2> m_webview2_2 = nullptr;
};

#endif // __WEBVIEW_BROWSER_API_HPP__