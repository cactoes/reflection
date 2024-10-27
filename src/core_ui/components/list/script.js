/**
 * @param {HTMLDivElement} parent
 * @param {HTMLDivElement} template
 * @param {string} name
 * @param {string[]} active_items
 * @param {string} callback_name
 */
create_functions["list"] = function (parent, template, name, active_items, callback_name) {
    const content = template.querySelector("div.content");

    for (let i = 0; i < active_items.length; i++) {
        const item = active_items[i];
        const p = document.createElement("p");
        p.innerText = item;
        p.onclick = () => {
            invoke(callback_name, [ i ]);
        }
        content.appendChild(p);
    }
};