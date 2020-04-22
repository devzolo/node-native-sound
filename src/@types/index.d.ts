declare module 'native-sound' {
  namespace native {
    interface NativeSoundCore {
      initialize(): void;
      init(): boolean;
      loadPlugin(path: string): void;
      NativeSound: NativeSound;
    }

    class NativeSound {
      getBPM(): void;

      getBufferLength(): void;

      getEffects(): void;

      getFFTData(): void;

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
  }
  const core: native.NativeSoundCore;
  export = core;
}
