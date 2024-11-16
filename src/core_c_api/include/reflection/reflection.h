#pragma once

#ifndef __REFLECTION_H__
#define __REFLECTION_H__

#ifdef REFLECTION_EXPORT
    #if __cplusplus >= 201703L
        #define REFLECTION_API extern "C" __declspec(dllexport) [[nodiscard]]
    #else
        #define REFLECTION_API extern "C" __declspec(dllexport)
    #endif
#else
    #if __cplusplus >= 201703L
        #define REFLECTION_API extern "C" __declspec(dllimport) [[nodiscard]]
    #else
        #define REFLECTION_API extern "C" __declspec(dllimport)
    #endif
#endif

// ~~ struct typedefs

typedef struct __WINDOW_OPTIONS {
    int width;
    int height;
    const char* name;
} window_options_t;

typedef enum __COMPONENT_FRAME_LAYOUT {
    fl_horizontal = 0,
    fl_vertical
} frame_layout_t;

typedef enum __REFLECTION_EVENT {
    E_ON_RENDER_START = 0,
    E_ON_RENDER_FINISHED
} event_t;

typedef enum __COMPONENT_FRAME_ALIGN {
    fa_none = 0,
    fa_horizontal,
    fa_vertical,
    fa_center,
} frame_align_t;

typedef struct __COMPONENT_FRAME_OPTIONS {
    bool is_tab_list = false;
    bool outline = true;
    frame_layout_t layout = frame_layout_t::fl_vertical;
    bool max_size = false;
    frame_align_t align = frame_align_t::fa_none;
    bool overflow = false;
    bool border = true;
} frame_options_t;

typedef struct __COMPONENT_BUTTON_OPTIONS {
    bool disabled = false;
    bool full_width = false;
} button_options_t;

typedef struct __COMPONENT_SELECTOR_OPTIONS {
    bool is_multi = false;
} selector_options_t;

typedef struct __COMPONENT_SLIDER_OPTIONS {
    bool show_ticks = true;
    const char* tick_name_left = "Low";
    const char* tick_name_right = "High";
} slider_options_t;

typedef struct __COMPONENT_INPUT_OPTIONS {
    const char* submit_button_text = "";
} input_options_t;

typedef struct __COMPONENT_UNSAFE_STRING {
    char* string;
    int size;
} unsafe_string_t;

typedef struct __COMPONENT_UNSAFE_INT_ARRAY {
    int* array;
    int size;
} unsafe_int_array_t;

// ~~ custom handle types for type safety

#define HANDLE_TYPE(name) struct __##name##_handle_t { int unused; }; typedef struct __##name##_handle_t *##name##_handle_t;

HANDLE_TYPE(browser);
HANDLE_TYPE(component);
HANDLE_TYPE(component_frame);
HANDLE_TYPE(component_label);
HANDLE_TYPE(component_button);
HANDLE_TYPE(component_checkbox);
HANDLE_TYPE(component_selector);
HANDLE_TYPE(component_slider);
HANDLE_TYPE(component_input);
HANDLE_TYPE(component_image);
HANDLE_TYPE(component_folder_selector);
HANDLE_TYPE(component_list);

// ~~ callback typedefs

typedef void(*event_handler_t)(const browser_handle_t);

typedef void(*button_callback_t)(component_button_handle_t);
typedef void(*checkbox_callback_t)(component_checkbox_handle_t, bool);
typedef void(*selector_callback_t)(component_selector_handle_t, const int*, int);
typedef void(*slider_callback_t)(component_slider_handle_t, int);
typedef const char*(*input_callback_t)(component_input_handle_t, const char*);
typedef void(*image_callback_t)(component_image_handle_t);
typedef void(*folder_selector_callback_t)(component_folder_selector_handle_t, const char*);
typedef void(*list_callback_t)(component_list_handle_t, int);

// ~~ function typedefs

REFLECTION_API void
destroy_string(
    unsafe_string_t* ptr);

REFLECTION_API void
destroy_array(
    unsafe_int_array_t* ptr);

REFLECTION_API browser_handle_t
init_browser(
    const window_options_t* window_options,
    const frame_options_t* frame_options);

REFLECTION_API void
browser_window_start(
    browser_handle_t handle);

REFLECTION_API component_frame_handle_t
browser_window_get_frame(
    browser_handle_t handle);

REFLECTION_API bool
browser_window_is_ready(
    browser_handle_t handle);

REFLECTION_API void
browser_window_register_event_handler(
    browser_handle_t handle,
    event_t event,
    event_handler_t event_handler);

REFLECTION_API void
browser_window_set_icon(
    browser_handle_t handle,
    int resource_id);

REFLECTION_API void
browser_window_set_color(
    browser_handle_t handle,
    const char* color);

REFLECTION_API void
browser_window_close(
    browser_handle_t handle);

REFLECTION_API void
component_set_target(
    component_handle_t handle,
    const char* id);

REFLECTION_API void
component_set_id(
    component_handle_t handle,
    const char* id);

REFLECTION_API void
component_set_name(
    component_handle_t handle,
    const char* name);

REFLECTION_API unsafe_string_t*
component_get_id(
    component_handle_t handle);

REFLECTION_API component_frame_handle_t
component_frame_add_frame(
    component_frame_handle_t handle,
    const char* name,
    const frame_options_t* options);

REFLECTION_API component_label_handle_t
component_frame_add_label(
    component_frame_handle_t handle,
    const char* text);

REFLECTION_API component_button_handle_t
component_frame_add_button(
    component_frame_handle_t handle,
    const char* name,
    button_callback_t callback,
    button_options_t* options);

REFLECTION_API component_checkbox_handle_t
component_frame_add_checkbox(
    component_frame_handle_t handle,
    const char* name,
    bool state,
    checkbox_callback_t callback);

REFLECTION_API component_selector_handle_t
component_frame_add_selector(
    component_frame_handle_t handle,
    const char* name,
    const char* list_options[],
    int list_options_size,
    int index[],
    int index_size,
    selector_callback_t callback,
    selector_options_t* options);

REFLECTION_API component_slider_handle_t
component_frame_add_slider(
    component_frame_handle_t handle,
    const char* name,
    int min,
    int max,
    int current,
    slider_callback_t callback,
    slider_options_t* options);

REFLECTION_API component_input_handle_t
component_frame_add_input(
    component_frame_handle_t handle,
    const char* value,
    input_callback_t callback,
    input_options_t* options);

REFLECTION_API component_image_handle_t
component_frame_add_image(
    component_frame_handle_t handle,
    const char* filepath,
    int width,
    int height,
    image_callback_t callback);

REFLECTION_API component_folder_selector_handle_t
component_frame_add_folder_selector(
    component_frame_handle_t handle,
    const char* name,
    const char* default_path,
    folder_selector_callback_t callback);

REFLECTION_API component_list_handle_t
component_frame_add_list(
    component_frame_handle_t handle,
    const char* name,
    const char* active_items[],
    int active_items_size,
    list_callback_t callback);

REFLECTION_API void
component_button_set_disabled(
    component_button_handle_t handle,
    bool state);

REFLECTION_API bool
component_checkbox_get_state(
    component_checkbox_handle_t handle);

REFLECTION_API unsafe_string_t*
component_folder_selector_get_path(
    component_folder_selector_handle_t handle);

REFLECTION_API unsafe_string_t*
component_input_get_value(
    component_input_handle_t handle);

REFLECTION_API void
component_list_set_items(
    component_list_handle_t handle,
    const char* items[],
    int size);

REFLECTION_API unsafe_int_array_t*
component_selector_get_active_index(
    component_selector_handle_t handle);

REFLECTION_API int
component_slider_get_value(
    component_slider_handle_t handle);

REFLECTION_API void
destroy_all_browsers();

REFLECTION_API void
destroy_browser(
    browser_handle_t* browser);

#endif // __REFLECTION_H__