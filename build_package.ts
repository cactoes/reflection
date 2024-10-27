import { $ } from "bun";
import * as fs from "fs";
import * as path from "path";
import archiver from "archiver";

await $`mkdir -p build && cd build && cmake .. -G "Visual Studio 17 2022" && cmake --build . --config Release --target reflection_core && cmake --build . --config Debug --target reflection_core`;

const dbg_path = path.join(__dirname, "build", "Debug", "reflection_core.lib");
const rel_path = path.join(__dirname, "build", "Release", "reflection_core.lib");

const archive = archiver("zip", {
    zlib: { level: 9 }
});

archive.pipe(fs.createWriteStream(__dirname + "/reflection-win-x64.zip"));
archive.append(fs.createReadStream(dbg_path), { name: "lib/reflection_core.debug.lib" });
archive.append(fs.createReadStream(rel_path), { name: "lib/reflection_core.release.lib" });
archive.directory(__dirname + "/src/core/include/reflection", "include/reflection");
archive.finalize();