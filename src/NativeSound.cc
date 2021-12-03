#include "NativeSound.hh"
#include <bass.h>
#include <bassmix.h>
#include "Utils.hh"

void CALLBACK BPMCallback(int handle, float bpm, void *user);
void CALLBACK BeatCallback(DWORD chan, double beatpos, void *user);

#define INVALID_FX_HANDLE (-1) // Hope that BASS doesn't use this as a valid Fx handle

HSTREAM NativeSound::ConvertFileToMono(const std::string &strPath)
{
  HSTREAM decoder = BASS_StreamCreateFile(FALSE, strPath.c_str(), 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_MONO); // open file for decoding  | BASS_UNICODE
  if (!decoder)
    return 0;                                                                       // failed
  DWORD length = static_cast<DWORD>(BASS_ChannelGetLength(decoder, BASS_POS_BYTE)); // get the length
  void *data = malloc(length);                                                      // allocate buffer for decoded data
  BASS_CHANNELINFO ci;
  BASS_ChannelGetInfo(decoder, &ci); // get sample format
  if (ci.chans > 1)                  // not mono, downmix...
  {
    HSTREAM mixer = BASS_Mixer_StreamCreate(ci.freq, 1, BASS_STREAM_DECODE | BASS_MIXER_END); // create mono mixer
    BASS_Mixer_StreamAddChannel(
        mixer, decoder, BASS_MIXER_DOWNMIX | BASS_MIXER_NORAMPIN | BASS_STREAM_AUTOFREE); // plug-in the decoder (auto-free with the mixer)
    decoder = mixer;                                                                      // decode from the mixer
  }
  length = BASS_ChannelGetData(decoder, data, length);                                         // decode data
  BASS_StreamFree(decoder);                                                                    // free the decoder/mixer
  HSTREAM stream = BASS_StreamCreate(ci.freq, 1, BASS_STREAM_AUTOFREE, STREAMPROC_PUSH, NULL); // create stream
  BASS_StreamPutData(stream, data, length);                                                    // set the stream data
  free(data);                                                                                  // free the buffer
  return stream;
}

Napi::FunctionReference NativeSound::constructor;

Napi::Object NativeSound::Init(Napi::Env env, Napi::Object exports)
{
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "NativeSound", {InstanceMethod("getBPM", &NativeSound::GetBPM), InstanceMethod("getBufferLength", &NativeSound::GetBufferLength), InstanceMethod("getEffects", &NativeSound::GetEffects), InstanceMethod("getFFTData", &NativeSound::GetFFTData), InstanceMethod("getLength", &NativeSound::GetLength), InstanceMethod("getLevelData", &NativeSound::GetLevelData), InstanceMethod("getMaxDistance", &NativeSound::GetMaxDistance), InstanceMethod("getMetaTags", &NativeSound::GetMetaTags), InstanceMethod("getMinDistance", &NativeSound::GetMinDistance), InstanceMethod("getPan", &NativeSound::GetPan), InstanceMethod("getPosition", &NativeSound::GetPosition), InstanceMethod("getProperties", &NativeSound::GetProperties), InstanceMethod("getSpeed", &NativeSound::GetSpeed), InstanceMethod("getVolume", &NativeSound::GetVolume), InstanceMethod("getWaveData", &NativeSound::GetWaveData), InstanceMethod("isPanningEnabled", &NativeSound::IsPanningEnabled), InstanceMethod("isPaused", &NativeSound::IsPaused), InstanceMethod("setEffectEnabled", &NativeSound::SetEffectEnabled), InstanceMethod("setMaxDistance", &NativeSound::SetMaxDistance), InstanceMethod("setMinDistance", &NativeSound::SetMinDistance), InstanceMethod("setPan", &NativeSound::SetPan), InstanceMethod("setPanningEnabled", &NativeSound::SetPanningEnabled), InstanceMethod("setPaused", &NativeSound::SetPaused), InstanceMethod("setPosition", &NativeSound::SetPosition), InstanceMethod("setProperties", &NativeSound::SetProperties), InstanceMethod("setSpeed", &NativeSound::SetSpeed), InstanceMethod("setVolume", &NativeSound::SetVolume), InstanceMethod("stop", &NativeSound::Stop)});

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("NativeSound", func);
  return exports;
}

NativeSound::NativeSound(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<NativeSound>(info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  int length = info.Length();

  if (length <= 0 || !info[0].IsString())
  {
    Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    return;
  }

  std::string strPath = info[0].As<Napi::String>().Utf8Value();

  BOOL b3D = FALSE;
  BOOL bLoop = FALSE;

  m_pSound = BASS_StreamCreateFile(FALSE, strPath.c_str(), 0, 0, BASS_SAMPLE_LOOP);

  if (!m_pSound)
    m_pSound = BASS_MusicLoad(FALSE, strPath.c_str(), 0, 0, BASS_MUSIC_RAMP | BASS_MUSIC_PRESCAN | BASS_STREAM_DECODE | BASS_UNICODE, 0); // Try again | BASS_UNICODE

  if (!m_pSound && b3D)
    m_pSound = NativeSound::ConvertFileToMono(strPath.c_str()); // Last try if 3D

  if (!m_pSound)
  {
    Napi::TypeError::New(
        env,
        StringFormat("BASS ERROR %d in LoadMedia  path:%s  3d:%d  loop:%d", BASS_ErrorGetCode(), strPath.c_str(), b3D, bLoop))
        .ThrowAsJavaScriptException();
    return;
  }

  BASS_ChannelPlay(m_pSound, FALSE);
}
/*
Napi::Value NativeSound::GetValue(const Napi::CallbackInfo& info) {
  double num = this->value_;

  return Napi::Number::New(info.Env(), num);
}

Napi::Value NativeSound::PlusOne(const Napi::CallbackInfo& info) {
  this->value_ = this->value_ + 1;

  return NativeSound::GetValue(info);
}

Napi::Value NativeSound::Multiply(const Napi::CallbackInfo& info) {
  Napi::Number multiple;
  if (info.Length() <= 0 || !info[0].IsNumber()) {
    multiple = Napi::Number::New(info.Env(), 1);
  } else {
    multiple = info[0].As<Napi::Number>();
  }

  Napi::Object obj = constructor.New(
      {Napi::Number::New(info.Env(), this->value_ * multiple.DoubleValue())});

  return obj;
}
*/

Napi::Value NativeSound::GetBPM(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (m_fBPM == 0.0f && !m_bStream)
  {
    float fData = 0.0f;

    // open the same file as played but for bpm decoding detection
    DWORD bpmChan = BASS_StreamCreateFile(false, m_strPath.c_str(), 0, 0, BASS_STREAM_DECODE | BASS_UNICODE);

    if (!bpmChan)
    {
      bpmChan = BASS_MusicLoad(false, m_strPath.c_str(), 0, 0, BASS_MUSIC_DECODE | BASS_MUSIC_PRESCAN | BASS_UNICODE, 0);
    }

    if (bpmChan)
    {
      double len = 0;
      if (m_pSound)
      {
        QWORD length = BASS_ChannelGetLength(m_pSound, BASS_POS_BYTE);
        if (length == -1)
          len = BASS_ChannelBytes2Seconds(m_pSound, length);
      }
      fData = BASS_FX_BPM_DecodeGet(bpmChan, 0, len, 0, BASS_FX_FREESOURCE, NULL, NULL);
      BASS_FX_BPM_Free(bpmChan);
    }

    if (BASS_ErrorGetCode() != BASS_OK)
    {
      Napi::TypeError::New(
          env,
          StringFormat("BASS ERROR %d in BASS_FX_BPM_DecodeGet  path:%s  3d:%d  loop:%d", BASS_ErrorGetCode(), m_strPath.c_str(), m_b3D, m_bLoop).c_str())
          .ThrowAsJavaScriptException();
    }
    else
    {
      m_fBPM = floor(fData);
    }
  }

  return Napi::Number::New(env, m_fBPM);
}

Napi::Value NativeSound::GetBufferLength(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::GetEffects(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::GetTempoValues(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  float fSampleRate;
  float fTempo;
  float fPitch;
  bool bReverse;
  fSampleRate = m_fSampleRate;
  fTempo = m_fTempo;
  fPitch = m_fPitch;
  bReverse = m_bReversed;
  return env.Undefined();
}

Napi::Value NativeSound::SetTempoValues(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  int length = info.Length();

  if (length <= 0 || !info[0].IsNumber())
  {
    Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (length <= 1 || !info[1].IsNumber())
  {
    Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (length <= 2 || !info[2].IsNumber())
  {
    Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (length <= 3 || !info[3].IsNumber())
  {
    Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  float fSampleRate = info[0].As<Napi::Number>().FloatValue();
  float fTempo = info[1].As<Napi::Number>().FloatValue();
  float fPitch = info[2].As<Napi::Number>().FloatValue();
  bool bReverse = info[3].As<Napi::Boolean>().Value();

  if (fTempo != m_fTempo)
  {
    m_fTempo = fTempo;
  }
  if (fPitch != m_fPitch)
  {
    m_fPitch = fPitch;
  }
  if (fSampleRate != m_fSampleRate)
  {
    m_fSampleRate = fSampleRate;
  }
  m_bReversed = bReverse;

  // Update our attributes
  if (m_pSound)
  {
    // TODO: These are lost when the sound is not streamed in
    BASS_ChannelSetAttribute(m_pSound, BASS_ATTRIB_TEMPO, m_fTempo);
    BASS_ChannelSetAttribute(m_pSound, BASS_ATTRIB_TEMPO_PITCH, m_fPitch);
    BASS_ChannelSetAttribute(m_pSound, BASS_ATTRIB_TEMPO_FREQ, m_fSampleRate);
    BASS_ChannelSetAttribute(BASS_FX_TempoGetSource(m_pSound), BASS_ATTRIB_REVERSE_DIR, (float)(bReverse == false ? BASS_FX_RVS_FORWARD : BASS_FX_RVS_REVERSE));
  }
  return env.Undefined();
}

Napi::Value NativeSound::GetFFTData(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  int length = info.Length();

  if (length <= 0 || !info[0].IsNumber())
  {
    Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
    return env.Null();
  }

  int iLength = info[0].As<Napi::Number>().Int32Value();

  if (m_pSound)
  {
    long lFlags = BASS_DATA_FFT256;
    if (iLength == 256)
      lFlags = BASS_DATA_FFT256;
    else if (iLength == 512)
      lFlags = BASS_DATA_FFT512;
    else if (iLength == 1024)
      lFlags = BASS_DATA_FFT1024;
    else if (iLength == 2048)
      lFlags = BASS_DATA_FFT2048;
    else if (iLength == 4096)
      lFlags = BASS_DATA_FFT4096;
    else if (iLength == 8192)
      lFlags = BASS_DATA_FFT8192;
    else if (iLength == 16384)
      lFlags = BASS_DATA_FFT16384;
    else
      return env.Null();

    float *pData = new float[iLength];

    int len = BASS_ChannelGetData(m_pSound, pData, lFlags);

    if (len != -1)
    {
      Napi::Array array = Napi::Array::New(env);
      for (int i = 0; i < iLength; i++)
      {
        array.Set(i, Napi::Number::New(env, pData[i]));
      }
      return array;
    }
    else
    {
      delete[] pData;
      Napi::TypeError::New(env, "BASS_ChannelGetData ERROR").ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }
  return env.Undefined();
}


// Non-streams only
Napi::Value NativeSound::GetLength(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  // Only relevant for non-streams, which are always ready if valid
  if (m_pSound)
  {
    QWORD length = BASS_ChannelGetLength(m_pSound, BASS_POS_BYTE);
    if (length == -1)
      return Napi::Number::New(env, BASS_ChannelBytes2Seconds(m_pSound, length));
  }
  return Napi::Number::New(env, 0);
}

Napi::Value NativeSound::GetLevelData(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (m_pSound)
  {
    DWORD dwData = BASS_ChannelGetLevel(m_pSound);
    if (dwData != 0)
      return Napi::Number::New(env, dwData);
  }
  return Napi::Number::New(env, 0);
}

Napi::Value NativeSound::GetMaxDistance(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::GetMetaTags(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::GetMinDistance(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::GetPan(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::GetPosition(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::GetProperties(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::GetSpeed(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::GetVolume(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::GetWaveData(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::IsPanningEnabled(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::IsPaused(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::SetEffectEnabled(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::SetMaxDistance(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::SetMinDistance(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::SetPan(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::SetPanningEnabled(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::SetPaused(const Napi::CallbackInfo &info)
{

  Napi::Env env = info.Env();

  int length = info.Length();

  if (length <= 0 || !info[0].IsBoolean())
  {
    Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
    return env.Null();
  }

  BOOL bPaused = info[0].As<Napi::Boolean>();

  m_bPaused = bPaused;

  if (m_pSound)
  {
    if (bPaused)
      BASS_ChannelPause(m_pSound);
    else
      BASS_ChannelPlay(m_pSound, FALSE);
  }
  return env.Undefined();
}

Napi::Value NativeSound::SetPosition(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::SetProperties(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::SetSpeed(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

Napi::Value NativeSound::SetVolume(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  float fVolume = info[0].As<Napi::Number>().FloatValue();

  m_fVolume = fVolume;

  if (m_pSound && !m_b3D)
    BASS_ChannelSetAttribute(m_pSound, BASS_ATTRIB_VOL, fVolume);

  return env.Undefined();
}

Napi::Value NativeSound::Stop(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return env.Undefined();
}

void CALLBACK BPMCallback(int handle, float bpm, void *user)
{
  /*
    NativeSound* pSound = LockCallbackId(user);
    if (pSound)
    {
        if (pSound->m_pVars)
        {
            pSound->m_pVars->criticalSection.Lock();
            pSound->m_pVars->onBPMQueue.push_back(bpm);
            pSound->m_pVars->criticalSection.Unlock();
        }
        else
        {
            pSound->SetSoundBPM(bpm);
        }
    }
    UnlockCallbackId();
  */
}

void CALLBACK BeatCallback(DWORD chan, double beatpos, void *user)
{
  /*
    NativeSound* pSound = LockCallbackId(user);
    if (pSound)
    {
        if (pSound->m_pVars)
        {
            pSound->m_pVars->criticalSection.Lock();
            pSound->m_pVars->onBeatQueue.push_back(beatpos);
            pSound->m_pVars->criticalSection.Unlock();
        }
    }
    UnlockCallbackId();
  */
}
