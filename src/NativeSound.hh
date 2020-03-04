#ifndef NATIVESOUND_H
#define NATIVESOUND_H

#include <napi.h>
#include <string>
#include <map>
#include <bass.h>
#include <bassmix.h>
#include <basswma.h>
#include <bass_fx.h>
#include "common.hh"

void CALLBACK BPMCallback(int handle, float bpm, void* user);
void CALLBACK BeatCallback(DWORD chan, double beatpos, void* user);

#define INVALID_FX_HANDLE (-1)  // Hope that BASS doesn't use this as a valid Fx handle

class NativeSound : public Napi::ObjectWrap<NativeSound> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  NativeSound(const Napi::CallbackInfo& info);

 private:

  static HSTREAM ConvertFileToMono(const std::string& strPath);

  static Napi::FunctionReference constructor;

  Napi::Value GetBPM(const Napi::CallbackInfo& info);
  Napi::Value GetBufferLength(const Napi::CallbackInfo& info);
  Napi::Value GetEffects(const Napi::CallbackInfo& info);
  Napi::Value GetFFTData(const Napi::CallbackInfo& info);
  Napi::Value GetLength(const Napi::CallbackInfo& info);
  Napi::Value GetLevelData(const Napi::CallbackInfo& info);
  Napi::Value GetMaxDistance(const Napi::CallbackInfo& info);
  Napi::Value GetMetaTags(const Napi::CallbackInfo& info);
  Napi::Value GetMinDistance(const Napi::CallbackInfo& info);
  Napi::Value GetPan(const Napi::CallbackInfo& info);
  Napi::Value GetPosition(const Napi::CallbackInfo& info);
  Napi::Value GetProperties(const Napi::CallbackInfo& info);
  Napi::Value GetSpeed(const Napi::CallbackInfo& info);
  Napi::Value GetVolume(const Napi::CallbackInfo& info);
  Napi::Value GetWaveData(const Napi::CallbackInfo& info);
  Napi::Value IsPanningEnabled(const Napi::CallbackInfo& info);
  Napi::Value IsPaused(const Napi::CallbackInfo& info);
  Napi::Value SetEffectEnabled(const Napi::CallbackInfo& info);
  Napi::Value SetMaxDistance(const Napi::CallbackInfo& info);
  Napi::Value SetMinDistance(const Napi::CallbackInfo& info);
  Napi::Value SetPan(const Napi::CallbackInfo& info);
  Napi::Value SetPanningEnabled(const Napi::CallbackInfo& info);
  Napi::Value SetPaused(const Napi::CallbackInfo& info);
  Napi::Value SetPosition(const Napi::CallbackInfo& info);
  Napi::Value SetProperties(const Napi::CallbackInfo& info);
  Napi::Value SetSpeed(const Napi::CallbackInfo& info);
  Napi::Value SetVolume(const Napi::CallbackInfo& info);
  Napi::Value Stop(const Napi::CallbackInfo& info);

  Napi::Value GetTempoValues(const Napi::CallbackInfo& info);
  Napi::Value SetTempoValues(const Napi::CallbackInfo& info);
public:
    //SSoundThreadVariables* m_pVars;
    uint32_t               uiEndSyncCount;
    bool                   bFreeSync;

private:
    const bool    m_bStream = false;
    const std::string m_strPath;
    const bool    m_b3D = false;
    const bool    m_bLoop = false;
    const bool    m_bThrottle = false;
    void*         m_pBuffer;
    unsigned int  m_uiBufferLength;

    bool    m_bPendingPlay;
    HSTREAM m_pSound;

    // Playback state
    bool    m_bPaused;
    bool    m_bReversed;
    bool    m_bPan;
    float   m_fDefaultFrequency;
    float   m_fVolume;
    float   m_fMinDistance;
    float   m_fMaxDistance;
    float   m_fPlaybackSpeed;
    float   m_fPitch;
    float   m_fTempo;
    float   m_fSampleRate;
    //CVector m_vecPosition;
    //CVector m_vecVelocity;

    //SFixedArray<int, 9> m_EnabledEffects;
    //SFixedArray<HFX, 9> m_FxEffects;

    // Info
    std::string                         m_strStreamName;
    std::string                         m_strStreamTitle;
    std::map<std::string, std::string>  m_ConvertedTagMap;

    //std::list<SSoundEventInfo> m_EventQueue;
    float                      m_fBPM;

    void* m_uiCallbackId;
    HSYNC m_hSyncDownload;
    HSYNC m_hSyncEnd;
    HSYNC m_hSyncFree;
    HSYNC m_hSyncMeta;
};

#endif //NATIVESOUND_H
