/**
 * @param {HTMLDivElement} parent
 * @param {HTMLDivElement} template
 * @param {string} name
 * @param {[ boolean, boolean, number, boolean, number, boolean, boolean ]} options
 */
create_functions["frame"] = function (parent, template, name, options) {
    const layout_map = [ "h", "v" ];
    const align_map = [ "", "ha", "va", "ac" ];
    const [ is_tab_list, outline, layout_index, max_size, align_index, overflow, border ] = options;
    const parent_is_tab_list = parent.classList.contains("tab");

    if (is_tab_list) {
        // make it a tablist
        template.classList.add("tab");

        // no need to do other stuff here
        // the tab list will not get a name
        return;
    }

    // if parent is tablist or normal tab
    template.classList.add(layout_map[layout_index] || "h");
    if (align_map[align_index])
        template.classList.add(align_map[align_index]);

    if (max_size)
        template.classList.add("size-max");

    if (overflow)
        template.classList.add("overflow");

    if (border)
        template.classList.add("border");

    if (parent_is_tab_list) {
        // get tab list header from parent
        const header = parent.querySelector(".tab_header");
        
        // create custom component
        const p_element = document.createElement("p");

        // if is first item make sure to hide other bodies
        // and set this first item as active
        if (parent.querySelector(".tab_header").children.length == 0)
            p_element.classList.add("active");
        else
            template.classList.add("hidden");

        // set header name
        p_element.innerHTML = name;
        p_element.id = "header_" + template.id;

        // add event
        p_element.onclick = (event) => {
            for (const _node of parent.querySelector(".tab_header").children)
                _node.classList.remove("active");

            for (const _node of parent.querySelector(".content").children)
                _node.classList.add("hidden");

            p_element.classList.add("active");
            template.classList.remove("hidden");
        };

        // append to header if it doesnt exist else replace
        const target = header.querySelector("p#" + "header_" + template.id);
        if (target == null) {
            header.appendChild(p_element);
        } else {
            if (target.classList.contains("active")) {
                p_element.classList.add("active");
                template.classList.remove("hidden");
            }
            header.replaceChild(p_element, target);
        }

        return;
    }

    if (outline) {
        template.classList.add("outline");
        
        if (name.length > 0) {
            const span = document.createElement("span");
            span.innerText = name;
            span.classList.add("noselect");
            template.appendChild(span);
        }
    }
};