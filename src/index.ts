import path  from "path";
const addonDir = path.resolve(`./bin/${process.platform}/${process.arch}/`);
const sound = require(path.join(`../bin/${process.platform}/${process.arch}/`,'native-sound')); //require('bindings')('example');

sound.initialize = function () {
    if(sound.init()) {
      // Load the Plugins
      let plugins = [
        "basswma.dll",
        "bassflac.dll",
        "bassmidi.dll",
        "bass_aac.dll",
        "bass_ac3.dll",
        "bassopus.dll",
      ];

      for(let plugin of plugins) {
        sound.loadPlugin(path.join(addonDir, plugin));
      }
    }
  return sound;
}

export default sound.initialize();

export const NativeSound = sound.NativeSound;
