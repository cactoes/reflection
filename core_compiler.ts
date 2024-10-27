import { minify, type MinifyOptions } from "terser";
import * as path from "path";
import * as fs from "fs";

const terser_config: MinifyOptions = {
    compress: {
        dead_code: true,
        drop_debugger: true,
        keep_fargs: true,
    },
    mangle: {
        toplevel: false
    },
    output: {
        comments: false
    }
};

const terser_config_debug: MinifyOptions = {
    compress: {
        dead_code: true,
        drop_debugger: true,
        keep_fargs: true,
        reduce_funcs: false,
        reduce_vars: false
    },
    mangle: false,
    output: {
        comments: false
    }
};

function minify_css(data: string): string {
    return data
        // comments
        .replace(/\/\*[\s\S]*?\*\//g, '')
        // whitespace before and after characters
        .replace(/\s*([{}:;,])\s*/g, '$1')
        // whitespace between selectors
        .replace(/\s*([^\{\};]+)\s*\{\s*([^\{\}]+)\s*\}/g, '$1{$2}')
        // whitespace between property-value pairs
        .replace(/\s*([^:;]+)\s*:\s*([^;]+)\s*;/g, '$1:$2;');
}

class timer {
    private m_start: number = 0;
    private m_end: number = 0;

    constructor() {}

    public start(): void {
        this.m_start = performance.now();
    }

    public stop(): void {
        this.m_end = performance.now();
    }

    public get time_ms(): number {
        return Math.round((this.m_end - this.m_start) * 10) / 10;
    }

    public static async timed(func: () => void | Promise<void>): Promise<timer> {
        const _timer = new timer();
        _timer.start();
        await func();
        _timer.stop();
        return _timer;
    }

    public static started(): timer {
        const _timer = new timer();
        _timer.start();
        return _timer;
    }
};

async function filter_files(_path: string, filter: (filename: string) => boolean, callback: (data: string, filename: string, buffer: Uint8Array) => Promise<void>) {
    const files = fs.readdirSync(_path).filter(filter);

    await Promise.all(files.map(async (_file_name) => {
        const _file_data = Bun.file(path.join(_path, _file_name));
        await callback(await _file_data.text(), _file_name, await _file_data.bytes());
    }));
}

async function prep_component(_path: string, component_name: string): Promise<{ style: string, layout: string, script: string }> {
    return {
        style: await Bun.file(path.join(_path, `style.css`)).text(),
        layout: `<template id="${component_name}_template">${await Bun.file(path.join(_path, `layout.html`)).text()}</template>`,
        script: await Bun.file(path.join(_path, `script.js`)).text()
    };
}

async function main(argc: number, argv: string[]): Promise<number> {
    if (argc != 3 || !["--debug", "--release"].includes(argv[2])) {
        console.log(`[core_compiler] no valid config given`);
        return -1;
    }

    const IS_DEV = argv[2] == "--debug";

    const core_ui_base_path = path.join(__dirname, "src/core_ui/");
    const core_compiler_temp = path.join(__dirname, "/build/core_compiler_temp.txt");

    const core_functions_path = path.join(core_ui_base_path, "/core_functions.js");
    const core_style_path = path.join(core_ui_base_path, "/core_style.css");
    const core_view_path = path.join(core_ui_base_path, "/core_view.html");
    const core_components_root_path = path.join(core_ui_base_path, "/components");
    const core_media_root_path = path.join(core_ui_base_path, "/media");

    console.log(`[core_compiler] devlopment mode: ${IS_DEV ? "true": "false"}`);
    console.log(`[core_compiler] core_ui: ${core_ui_base_path}`);

    const out_data = { style: "", layout: "", script: "" };

    const total_timer = timer.started();

    const timer_load_files = timer.started();

    const component_files = fs.readdirSync(core_components_root_path).filter(file => fs.lstatSync(path.join(core_components_root_path, file)).isDirectory());
    const components_data = await Promise.all(component_files.map(file => prep_component(path.join(core_components_root_path, file), file)));
    components_data.forEach(component_data => {
        out_data.style += component_data.style;
        out_data.layout += component_data.layout;
        out_data.script += component_data.script;
    });

    if (!fs.existsSync(core_compiler_temp)) {
        fs.mkdirSync(path.join(core_compiler_temp, ".."), { recursive: true });
        fs.writeFileSync(core_compiler_temp, "");
    }

    let [ core_functions_data, core_style_data, core_view_data, stored_hashes ] = await Promise.all([
        Bun.file(core_functions_path).text(),
        Bun.file(core_style_path).text(),
        Bun.file(core_view_path).text(),
        Bun.file(core_compiler_temp).text()
    ]);

    core_functions_data = core_functions_data.replace("const IS_DEV = true;", `const IS_DEV = ${IS_DEV};`)

    // const hash_core_functions_data = new Bun.SHA256().update(core_functions_data).digest("base64");
    // const hash_core_style_data = new Bun.SHA256().update(core_style_data).digest("base64");
    // const hash_core_view_data = new Bun.SHA256().update(core_view_data).digest("base64");
    // const hash_components =  new Bun.SHA256().update(JSON.stringify(out_data)).digest("base64");

    // const new_hashes = hash_core_functions_data + hash_core_style_data + hash_core_view_data + hash_components;

    timer_load_files.stop();
    console.log(`[core_compiler (${timer_load_files.time_ms.toString().padEnd(4, " ")} ms)] loaded all files`);

    // if (stored_hashes == new_hashes) {
    //     console.log("[core_compiler] no files changed");
    //     return 0;
    // }

    const timer_parse_css = timer.started();

    // prep css
    const core_style_html = `<style>${minify_css(core_style_data + out_data.style)}</style>`;

    timer_parse_css.stop();
    console.log(`[core_compiler (${timer_parse_css.time_ms.toString().padEnd(4, " ")} ms)] parsed css`);

    const timer_parse_html = timer.started();

    // prep html
    core_view_data = core_view_data.replace("<style></style>", core_style_html);
    core_view_data += out_data.layout;

    // -> load images
    await filter_files(core_media_root_path, n => n.endsWith(".png"), async (_data, name, buffer) => {
        const out_buffer = Buffer.from(buffer).toString("base64");
        core_view_data = core_view_data.replace(`<img src="media/${name}">`, `<img src="data:image/png;base64,${out_buffer}">`);
    });

    timer_parse_html.stop();
    console.log(`[core_compiler (${timer_parse_html.time_ms.toString().padEnd(4, " ")} ms)] parsed html`);

    const timer_parse_js = timer.started();

    // js
    core_functions_data = `document.getElementsByTagName("html")[0].innerHTML = \`${core_view_data}\`;\n` + core_functions_data;
    core_functions_data += out_data.script;
    const core_functions_min = await minify(core_functions_data, IS_DEV ? terser_config_debug : terser_config);
    
    timer_parse_js.stop();
    console.log(`[core_compiler (${timer_parse_js.time_ms.toString().padEnd(4, " ")} ms)] parsed js`);

    const core_timer = timer.started();

    // stupid formatter so we dont need to deal with escaping quotes
    const buffer = Buffer.from(core_functions_min.code!);
    let outstr = "";
    for (let i = 0; i < buffer.length; i++) {
        outstr += (buffer.at(i)!.toString() + ",").padEnd(5, " ");

        if (((i + 1) % 20) == 0 && i != buffer.length - 1)
            outstr += "\n    ";
    }

    const file_data = `#pragma once
#ifndef __CORE_UI_HPP__
#define __CORE_UI_HPP__

#include <string>

template <size_t _size>
std::string format_byte_array(const char (&byte_array)[_size]) {
    return std::string(byte_array, _size);
}

static const char core_ui[] = {
    ${outstr}
};

#endif // __CORE_UI_HPP__`;

    await Bun.write("src/ext/include/ext/core_ui.hpp", file_data);

    // fs.writeFileSync(core_compiler_temp, new_hashes);

    core_timer.stop();

    const size_kb = buffer.length / 1000;
    console.log(`[core_compiler (${core_timer.time_ms.toString().padEnd(4, " ")} ms)] wrote buffer (${size_kb} kb)`);

    total_timer.stop();

    console.log("[core_compiler] successfully compiled core_ui in", total_timer.time_ms, "ms");

    return 0;
}

process.exit(await main(process.argv.length, process.argv));