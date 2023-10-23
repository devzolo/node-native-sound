declare module '@devzolo/node-native-sound' {

  interface NativeSoundCore {
    initialize(): void;
    init(): boolean;
    loadPlugin(path: string): void;
  }

  export class NativeSound {

    constructor(path: string);

    getBPM(): void;

    getBufferLength(): void;

    getEffects(): void;

    getFFTData(length: 256 | 512 | 1024 | 2048 | 4096 | 8192 | 16384): number[];

    getLength(): void;

    getLevelData(): void;

    getMaxDistance(): void;

    getMetaTags(): void;

    getMinDistance(): void;

    getPan(): void;

    getPosition(): void;

    getProperties(): void;

    getSpeed(): void;

    getVolume(): void;

    getWaveData(): void;

    isPanningEnabled(): void;

    isPaused(): void;

    setEffectEnabled(): void;

    setMaxDistance(): void;

    setMinDistance(): void;

    setPan(): void;

    setPanningEnabled(): void;

    setPaused(): void;

    setPosition(): void;

    setProperties(): void;

    setSpeed(): void;

    setVolume(): void;

    stop(): void;
  }

  const core: NativeSoundCore;
  export default core;
}
