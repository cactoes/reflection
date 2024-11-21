/**
 * @param {HTMLDivElement} parent
 * @param {HTMLDivElement} template
 * @param {string} name
 * @param {number} width
 * @param {number} height
 * @param {string} callback_name
 */
create_functions["canvas"] = async function (parent, template, name, width, height, callback_name) {
    const canvas = template.querySelector("canvas");
    const context = canvas.getContext("2d");

    canvas.width = width;
    canvas.height = height;

    canvas_renders.push(() => {
        const buffer = canvas_buffers[template.id];
        if (buffer)
            context.putImageData(buffer, 0, 0);
    });
};

/**
 * @param {HTMLDivElement} element
 * @param {number[]} buffer
 */
update_functions["canvas"] = function (id, element, buffer, width, height) {
    canvas_buffers[id] = new ImageData(new Uint8ClampedArray((new Uint32Array(buffer)).buffer), width, height);
};