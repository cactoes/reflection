/**
 * @param {HTMLDivElement} parent
 * @param {HTMLDivElement} template
 * @param {string} name
 * @param {string} callback_name
 * @param {[ boolean, boolean ]} options
 */
create_functions["button"] = function (parent, template, name, callback_name, options) {
    const [ disabled, full_width ] = options;

    if (full_width)
        template.classList.add("full-width");

    const button_element = template.querySelector(".content");
    if (disabled) {
        button_element.classList.add("disabled");
        button_element.disabled = "true";
    }

    button_element.innerText = name;
    button_element.onclick = () => {
        invoke(callback_name, []);
    }
};