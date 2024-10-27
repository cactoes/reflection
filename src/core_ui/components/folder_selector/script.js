/**
 * @param {HTMLDivElement} parent
 * @param {HTMLDivElement} template
 * @param {string} name
 * @param {string} path
 * @param {string} callback_name
 */
create_functions["folder_selector"] = function (parent, template, name, path, callback_name) {
    const input = template.querySelector("input");
    const button = template.querySelector("button");

    input.value = path;

    button.onclick = async () => {
        input.value = await __core_open_folder_selector__(input.value);
        invoke(callback_name, [ input.value ]);
    }

    input.onblur = () => {
        invoke(callback_name, [ input.value ]);
    }
};