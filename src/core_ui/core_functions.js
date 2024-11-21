// ====================
// SECTION: globals & consts
// ====================
const IS_DEV = true;
const MB_MIDDLE = 1;
const HEX_CODE_REGEX = /^#([a-fA-F0-9]{6})$/;

// ====================
// SECTION: window move / drag & cursor hooks + other window events
// ====================
const __mouse_data__ = {
    pos: {
        x: 0,
        y: 0,
    },
    isDragging: false,
};

document.addEventListener("mousedown", (mouseEvent) => {
    if (MB_MIDDLE == mouseEvent.button || getComputedStyle(mouseEvent.target, null).getPropertyValue("--webkit-app-region") == "drag") {
        __mouse_data__.isDragging = true;
        __mouse_data__.pos.x = mouseEvent.clientX
        __mouse_data__.pos.y = mouseEvent.clientY;
        document.getElementsByTagName("body")[0].style.cursor = "grabbing";
    }
});

document.addEventListener("mousemove", (mouseEvent) => {
    if (__mouse_data__.isDragging) {
        __core_post_we__("we_move_window",
            mouseEvent.clientX - __mouse_data__.pos.x,
            mouseEvent.clientY - __mouse_data__.pos.y);
    }
});

document.addEventListener("mouseup", () => {
    if (__mouse_data__.isDragging) {
        __mouse_data__.isDragging = false;
        document.getElementsByTagName("body")[0].style.cursor = "";
    }
});

document.addEventListener("click", (event) => {
    // close selector when not clicking in selector or on other selector
    if (!event.target.parentElement.parentElement.classList.contains("selector")) {
        for (const element of document.querySelectorAll(".selector>div.content.active")) {
            if (event.target.classList.contains("header")) {
                if (event.target.parentElement.parentElement.querySelector(".selector>div.content.active") != element)
                    element.classList.remove("active");
            } else {
                element.classList.remove("active");
            }
        }
    } else {
        for (const element of document.querySelectorAll(".selector>div.content.active")) {
            if (event.target.parentElement.parentElement.querySelector(".selector>div.content.active") != element)
                element.classList.remove("active");
        }
    }
})

document.addEventListener("keydown", e => (e.key == "F5") && e.preventDefault());

for (const button of document.getElementsByClassName("we_button")) {
    button.addEventListener("click", event => {
        __core_post_we__(`we_${event.currentTarget.id}`);
    });
}

document.addEventListener("contextmenu", e => (!IS_DEV) && e.preventDefault());

// ====================
// SECTION: cpp invokers & js invokers
// ====================
const __function_data__ = {
    resultMap: new Map(),
    registerdFunctions: {}
};

function __core_result_handler__(name, result) {
    __function_data__.resultMap.get(name).resolve(result);
    __function_data__.resultMap.delete(name);
}

function __core_call_js__(name, ...args) {
    if (__function_data__.registerdFunctions[name])
        __function_data__.registerdFunctions[name](...args);
    else if (IS_DEV)
        console.error(`function ${name} not registered`);
}

async function __core_open_folder_selector__(path) {
    return await __core_post_we__("we_open_folder_selector", path ? path : "C:\\");
}

async function invoke(name, args) {
    return new Promise((reslove, reject) => {
        if (__function_data__.resultMap.get(name))
            return  __function_data__.resultMap.get(name);

        __function_data__.resultMap.set(name, { resolve: reslove, reject: reject });
        window.chrome.webview.postMessage({ __name__: name, __args__: args });
    });
}

function register(func) {
    __function_data__.registerdFunctions[func.name] = func;
}

function __core_set_title__(name) {
    if (document.getElementById("title"))
        document.getElementById("title").innerText = name;
}

function __set_title_wrapper__(name) {
    __core_set_title__(name);
}

function __core_set_icon__(buffer_image_data) {
    if (document.getElementById("icon"))
        document.getElementById("icon").src = `data:image/x-icon;base64,${buffer_image_data}`;
}

function __set_icon_wrapper__(buffer_image_data) {
    __core_set_icon__(buffer_image_data);
}

function __core_set_color__(hex_code) {
    if (HEX_CODE_REGEX.test(hex_code))
        document.documentElement.style.setProperty("--primary", hex_code);
}

function __set_color_wrapper__(hex_code) {
    __core_set_color__(hex_code);
}

const __core_window_events__ = [
    "we_move_window",
    "we_open_folder_selector",
    "we_minimize",
    "we_resize",
    "we_close"
];

async function __core_post_we__(we, ...args) {
    if (!__core_window_events__.includes(we))
        return false;
    
    // can return a bunch of items
    return await invoke("handle_window_events", [ we, ...args ]);
}

// ====================
// SECTION: ui functions
// ====================
const create_functions = {};
const update_functions = {};
const canvas_renders = [];
const canvas_buffers = {};

function render_canvas_loop() {
    for (const canvas_render of canvas_renders)
        canvas_render();

    requestAnimationFrame(render_canvas_loop);
}

function parse_args(args) {
    return args.map((item, index) => { return { [`a${index}`]: JSON.stringify(item) } });
}

function fromat_args(base,  target, id, name, args) {
    args = Object.assign({ target: JSON.stringify(target), id: JSON.stringify(id), name: JSON.stringify(name) }, ...args);
    return Object.keys(args).map(item => {
        return `\n${Array.from({ length: base - item.length }).join(" ")}${item}: ${args[item]}`;
    });
}

async function update_ui_component_data(type, id, ...args) {
    if (update_functions[type] == undefined) {
        if (IS_DEV)
            console.error(`ui component update "${type}" not registered`);
        return;
    }

    if (!document.getElementById(id)) {
        if (IS_DEV)
            console.error(`target "${id}" not found`);
        return;
    }

    const item = document.getElementById(id);

    try {
        await update_functions[type].apply(null, [ id, item, ...args ]);
    } catch (error) {
        if (IS_DEV)
            console.error(error);
        return;
    }
}

async function create_ui_component(type, target, id, name, ...args) {
    if (IS_DEV)
        console.log(`[creating (${type})]${fromat_args(10, target, id, name, parse_args(args))}`);

    if (create_functions[type] == undefined) {
        if (IS_DEV)
            console.error(`ui component "${type}" not registered`);
        return;
    }
    
    if (!document.getElementById(target)) {
        if (IS_DEV)
            console.error(`target "${target}" not found`);
        return;
    }

    // copy the template
    const item = document.importNode(document.getElementById(type + "_template").content.childNodes[0], true);
    item.id = id;

    const target_element = document.getElementById(target);

    // add functionality
    try {
        await create_functions[type].apply(null, [ target_element, item, name, ...args ]);
    } catch (error) {
        if (IS_DEV)
            console.error(error);
        return;
    }

    // item already exists so just replace it
    if (document.getElementById(id)) {
        const element = document.getElementById(id);
        element.parentElement.replaceChild(item, element);
        return;
    }

    // append item
    if (target_element.getElementsByClassName("content").length > 0)
        target_element.getElementsByClassName("content")[0].appendChild(item);
    else
        target_element.appendChild(item);
}

register(create_ui_component);
register(update_ui_component_data);
register(__set_icon_wrapper__);
register(__set_title_wrapper__);
register(__set_color_wrapper__);
invoke("render_layout", []).then(() => {
    invoke("on_render_finished", []);
    render_canvas_loop();
});