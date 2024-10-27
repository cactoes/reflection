/**
 * @param {HTMLDivElement} parent
 * @param {HTMLDivElement} template
 * @param {string} name
 */
create_functions["label"] = function (parent, template, name) {
    template.querySelector(".content").innerText = name;
};