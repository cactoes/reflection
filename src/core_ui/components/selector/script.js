/**
 * @param {HTMLDivElement} parent
 * @param {HTMLDivElement} template
 * @param {string} name
 * @param {string[]} list_options
 * @param {number[]} index
 * @param {string} callback_name
 * @param {[ boolean ]} list_options
 */
create_functions["selector"] = function (parent, template, name, list_options, index, callback_name, options) {
    const [ is_multi ] = options;
    const active_item = template.querySelector("p.active-item");
    const content = template.querySelector("div.content");

    if (is_multi)
        active_item.innerText = list_options.filter((opt, i) => index.includes(i)).join(", ");
    else
        active_item.innerText = list_options[index[0]];

    if (index.length == 0)
        active_item.innerText = "(Empty)";

    for (const option_index in list_options) {
        const option = list_options[option_index];
        const p = document.createElement("p");
        
        p.innerText = option;
        p.classList.add("noselect");
        if (index.includes(parseInt(option_index)))
            p.classList.add("active");

        p.onclick = () => {
            if (!is_multi)
                content.classList.remove("active");

            if (is_multi) {
                p.classList.contains("active") ? p.classList.remove("active") : p.classList.add("active");

                const indexes = []
                content.querySelectorAll("p").forEach((_item, _index) => _item.classList.contains("active") && indexes.push(_index));

                active_item.innerText = indexes.map((i) => list_options[i]).join(", ");

                if (indexes.length == 0)
                    active_item.innerText = "(Empty)";

                invoke(callback_name, [ indexes ]);
            } else {
                for (const _p_child of content.querySelectorAll("p"))
                    _p_child.classList.remove("active");

                active_item.innerText = option;
                invoke(callback_name, [ [ parseInt(option_index) ] ]);
                p.classList.add("active");
            }
        }
        content.appendChild(p);
    }

    const header = template.querySelector("div.header");
    header.onclick = () => {
        if (content.classList.contains("active"))
            content.classList.remove("active");
        else
            content.classList.add("active");
    }
};