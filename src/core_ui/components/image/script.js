/**
 * @param {HTMLDivElement} parent
 * @param {HTMLDivElement} template
 * @param {string} filepath
 * @param {number} width
 * @param {number} height
 * @param {string} callback_name
 */
create_functions["image"] = function (parent, template, filedata, width, height, callback_name) {
    const image = template.querySelector("img");
    image.src = `data:image/png;base64,${filedata}`;
    image.style.width = `${width}px`;
    image.style.height = `${height}px`;
    image.onclick = () => {
        invoke(callback_name, []);
    }
};