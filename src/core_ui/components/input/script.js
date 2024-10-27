/**
 * @param {HTMLDivElement} parent
 * @param {HTMLDivElement} template
 * @param {string} value
 * @param {string} callback_name
 * @param {[ string ]} options
 */
create_functions["input"] = function (parent, template, value, callback_name, options) {
    const [ submit_button_text ] = options;

    const input = template.querySelector("input");

    if (submit_button_text.length != 0) {
        const btn = document.createElement("button");
        btn.innerText = submit_button_text;
        btn.onclick = async () => {
            input.value = await invoke(callback_name, [ input.value ]);
        }

        template.appendChild(btn);
    }

    input.value = value;
    input.onblur = async () => {
        if (submit_button_text.length == 0)
            input.value = await invoke(callback_name, [ input.value ]);
    }
};