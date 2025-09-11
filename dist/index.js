import { createRequire } from "node:module";
import path from "node:path";
const require = createRequire(import.meta.url);
const addonDir = path.resolve(path.join(import.meta.dirname, "..", "bin", process.platform, process.arch));
const sound = require(path.join(import.meta.dirname, "..", "bin", process.platform, process.arch, "native-sound.node"));
sound.initialize = () => {
    if (sound.init()) {
        // Load the Plugins
        const plugins = [
            "basswma.dll",
            "bassflac.dll",
            "bassmidi.dll",
            "bass_aac.dll",
            "bass_ac3.dll",
            "bassopus.dll",
        ];
        for (const plugin of plugins) {
            sound.loadPlugin(path.join(addonDir, plugin));
        }
    }
    return sound;
};
export default sound.initialize();
export const { NativeSound } = sound;
//# sourceMappingURL=index.js.map