import path from 'path';

const addonDir = path.resolve(path.join(__dirname, "..", "bin", process.platform, process.arch));

const sound = require(path.join(__dirname, "..", "bin", process.platform, process.arch, 'native-sound'));

sound.initialize = (): unknown => {
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
      sound.loadPlugin(path.join(addonDir, plugin));
    }
  }
  return sound;
};

export default sound.initialize();

export const { NativeSound } = sound;

