/**
 * @param {HTMLDivElement} parent
 * @param {HTMLDivElement} template
 * @param {string} name
 * @param {boolean} state
 * @param {string} callback_name
 */
create_functions["checkbox"] = function (parent, template, name, state, callback_name) {
    template.querySelector("p.title").innerText = name;
    
    const input = template.querySelector("input");
    input.checked = state;
    input.onclick = () => {
        invoke(callback_name, [ input.checked ]);
    };
};