/**
 * @param {HTMLDivElement} parent
 * @param {HTMLDivElement} template
 * @param {string} name
 * @param {number} min
 * @param {number} max
 * @param {number} current
 * @param {string} callback_name
 * @param {[ boolean, string, string ]} options
 */
create_functions["slider"] = function (parent, template, name, min, max, current, callback_name, options) {
    const [ show_ticks, tick_name_left, tick_name_right ] = options;

    template.querySelector("p.left").innerText = tick_name_left;
    template.querySelector("p.right").innerText = tick_name_right;
    
    const input = template.querySelector("input");
    input.min = `${min}`;
    input.max = `${max}`;
    input.value = `${current}`;
    input.onchange = () => {
        invoke(callback_name, [ parseInt(input.value) ]);
    }

    if (show_ticks) {
        const ticks = template.querySelector("div.ticks");
        for (let i = 0; i < max + 1; i++) {
            const tick = document.createElement("div");
            tick.className = "tick";
            ticks.appendChild(tick);
        }
    }
};