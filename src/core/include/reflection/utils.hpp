#pragma once

#ifndef __REFLECTION_UTILS_HPP__
#define __REFLECTION_UTILS_HPP__

#define WIN32_LEAN_AND_MEAN

#include <stdint.h>
#include <string>
#include <fstream>
#include <random>
#include <vector>
#include <any>
#include <optional>
#include <functional>
#include <type_traits>
#include <Windows.h>
#include <queue>
#include <mutex>

#if defined(_DEBUG) || !defined(NDEBUG)
    #define REFLECTION_DBG true
#else
    #define REFLECTION_DBG false
#endif

#define ru_hash_fnv1a_64 reflection::utils::hash::fnv1a_64
#define ru_hash_string_64 reflection::utils::hash::string_64
#define ru_hash_base64 reflection::utils::hash::base64_encode

#define ru_str_to_wstr reflection::utils::string_to_wstring
#define ru_wstr_to_str reflection::utils::wstring_to_string

#define ru_random_uuid reflection::utils::random::uuid

#define ru_function reflection::utils::func_ptr

namespace reflection::utils {
    template <typename _return_type, typename... _arguments>
    class func_ptr {
    public:
        func_ptr() : m_func(std::nullopt) {}

        template <typename _class>
        func_ptr(_return_type(_class::*func)(_arguments...), _class* obj) {
            func_ptr([obj, func](_arguments&&... args) { return (obj->*func)(std::forward<_arguments>(args)...); });
        }

    #if __cplusplus >= 202002L
        template <typename _function>
        requires std::is_invocable_r_v<_return_type, _function, _arguments...>
        func_ptr(_function&& func)
            : m_func(std::forward<_function>(func)) {}

    #else
        func_ptr(std::function<_return_type(_arguments...)> func)
            : m_func(func) {}
    #endif

        std::optional<std::function<_return_type(_arguments...)>> get() {
            return m_func;
        }

        template <typename T = _return_type, typename std::enable_if_t<!std::is_same_v<T, void>>* = nullptr>
        std::optional<_return_type> operator()(_arguments... args) {
            return m_func.has_value()
                ? std::optional<_return_type>{ (*m_func)(std::forward<_arguments>(args)...) }
                : std::nullopt;
        }

        template <typename T = _return_type, typename std::enable_if_t<std::is_same_v<T, void>>* = nullptr>
        void operator()(_arguments... args) {
            if (m_func.has_value())
                (*m_func)(std::forward<_arguments>(args)...);
        }

    private:
        std::optional<std::function<_return_type(_arguments...)>> m_func;
    };

    template <typename _object>
    class mt_bus {
    public:
        mt_bus() = default;

        void post(const _object& object) {
            std::lock_guard<std::mutex> guard(m_lock);
            m_bus.push(object);
        }

        bool get(_object& object) {
            std::lock_guard<std::mutex> guard(m_lock);

            if (m_bus.empty())
                return false;

            object = m_bus.front();
            m_bus.pop();

            return true;
        }

        std::optional<_object> pop_front() {
            std::lock_guard<std::mutex> guard(m_lock);
            if (m_bus.empty())
                return std::nullopt;

            _object _front = m_bus.front();
            m_bus.pop();
            return _front;
        }

    private:
        std::queue<_object> m_bus = {};
        std::mutex m_lock = {};
    };

    inline std::wstring string_to_wstring(const std::string_view string) {
        if (string.empty())
            return {};

        const int stringLength = MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), nullptr, 0);
        std::wstring wstring(stringLength, 0);
        MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), wstring.data(), stringLength);
        return wstring;
    }

    inline std::string wstring_to_string(const std::wstring_view wstring) {
        if (wstring.empty())
            return {};

        const int stringLength = WideCharToMultiByte(CP_UTF8, 0, wstring.data(), static_cast<int>(wstring.size()), nullptr, 0, nullptr, nullptr);
        std::string string(stringLength, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstring.data(), static_cast<int>(wstring.size()), string.data(), stringLength, nullptr, nullptr);
        return string;
    }

    inline std::string replace(std::string str, const std::string& from, const std::string& to) {
        size_t startPos = 0;
        while ((startPos = str.find(from, startPos)) != std::string::npos) {
            str.replace(startPos, from.length(), to);
            startPos += to.length();
        }
        return str;
    }

    inline std::optional<std::vector<uint8_t>> file_to_buffer(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file)
            return std::nullopt;

        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(size);
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
            return std::nullopt;

        return buffer;
    }

    inline std::string get_window_title(HWND window) {
        char title[256];
        if (GetWindowTextA(window, title, sizeof(title)) > 0)
            return std::string(title);
        else
            return std::string("");
    }

    inline std::optional<std::vector<uint8_t>> load_resource(int resource_id, HMODULE module_handle = GetModuleHandle(nullptr)) {
        HRSRC resource_handle = FindResourceA(module_handle, MAKEINTRESOURCE(resource_id), RT_ICON);
        if (!resource_handle)
            return std::nullopt;

        HGLOBAL loaded_resource_handle = LoadResource(module_handle, resource_handle);
        if (!loaded_resource_handle)
            return std::nullopt;

        void* resource_data = LockResource(loaded_resource_handle);
        if (!resource_data)
            return std::nullopt;

        DWORD resource_size = SizeofResource(module_handle, resource_handle);
        if (resource_size == 0)
            return std::nullopt;

        return std::vector<uint8_t>{ static_cast<uint8_t*>(resource_data), static_cast<uint8_t*>(resource_data) + resource_size };
    }

    namespace random {
        template <int _size = 8>
        inline std::string string(int count = _size) {
            const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, (int)charset.length() - 1);

            std::string randomString;
            randomString.reserve(count);

            for (int i = 0; i < count; ++i)
                randomString += charset[dis(gen)];

            return randomString;
        }

        template <int _min = 0, int _max = 10>
        inline int number(int min = _min, int max = _max) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(min, max);
            return dis(gen);
        }

        inline std::string uuid() {
            return string(number<4, 10>()) + "-" + string(number<4, 10>()) + "-" + string(number<4, 10>()) + "-" + string(number<4, 10>());
        }
    } // namespace random

    namespace hash {
        constexpr uint64_t fnv1a_64(const char* str, uint64_t hash = 14695981039346656037ULL) {
            return (*str == '\0') ? hash :
                fnv1a_64(str + 1, (hash ^ static_cast<uint64_t>(*str)) * 1099511628211ULL);
        }

        constexpr uint64_t string_64(const char* str, uint64_t hash = 0ULL) {
            return (*str == '\0') ? hash :
                string_64(str + 1, (hash << 1) + static_cast<uint64_t>(*str));
        }

        static const unsigned char base64_table[65] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        inline std::string base64_encode(const unsigned char *src, size_t len) {
            unsigned char *out, *pos;
            const unsigned char *end, *in;

            size_t olen;

            olen = 4*((len + 2) / 3); /* 3-byte blocks to 4-byte */

            if (olen < len)
                return std::string(); /* integer overflow */

            std::string outStr;
            outStr.resize(olen);
            out = (unsigned char*)&outStr[0];

            end = src + len;
            in = src;
            pos = out;
            while (end - in >= 3) {
                *pos++ = base64_table[in[0] >> 2];
                *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
                *pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
                *pos++ = base64_table[in[2] & 0x3f];
                in += 3;
            }

            if (end - in) {
                *pos++ = base64_table[in[0] >> 2];
                if (end - in == 1) {
                    *pos++ = base64_table[(in[0] & 0x03) << 4];
                    *pos++ = '=';
                }
                else {
                    *pos++ = base64_table[((in[0] & 0x03) << 4) |
                        (in[1] >> 4)];
                    *pos++ = base64_table[(in[1] & 0x0f) << 2];
                }
                *pos++ = '=';
            }

            return outStr;
        }

        inline std::string base64_encode(const std::vector<uint8_t>& buffer) {
            return base64_encode(buffer.data(), buffer.size());
        }
    } // namespace hash
} // namespace reflection::utils

#endif // __REFLECTION_UTILS_HPP__