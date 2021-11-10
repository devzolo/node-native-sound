"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.NativeSound = void 0;
const path_1 = __importDefault(require("path"));
const addonDir = path_1.default.resolve(path_1.default.join(__dirname, "..", "bin", process.platform, process.arch));
const sound = require(path_1.default.join(__dirname, "..", "bin", process.platform, process.arch, 'native-sound'));
sound.initialize = () => {
    if (sound.init()) {
        // Load the Plugins
        const plugins = [
            'basswma.dll',
            'bassflac.dll',
            'bassmidi.dll',
            'bass_aac.dll',
            'bass_ac3.dll',
            'bassopus.dll',
        ];
        for (const plugin of plugins) {
            sound.loadPlugin(path_1.default.join(addonDir, plugin));
        }
    }
    return sound;
};
exports.default = sound.initialize();
exports.NativeSound = sound.NativeSound;
