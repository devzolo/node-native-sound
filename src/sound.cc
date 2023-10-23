#include <napi.h>
#include <bass.h>
#include <bassmix.h>
#include "Common.hh"
#include "Utils.hh"
#include "NativeSound.hh"


namespace sound {
/*
#include <shlwapi.h>
std::string  thisDllDirPath()
{
    std::string thisPath;
    CHAR path[MAX_PATH];
    HMODULE hm;
    if( GetModuleHandleExA( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPSTR) &thisDllDirPath, &hm ) )
    {
        GetModuleFileNameA( hm, path, sizeof(path) );
        PathRemoveFileSpecA( path );
        thisPath = std::string( path );
        if( !thisPath.empty() &&
            thisPath.at( thisPath.length()-1 ) != '\\' )
            thisPath += "\\";
    }
    return thisPath;
}
*/


Napi::Value init(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  // Initialize BASS audio library
  if (!BASS_Init(-1, 44100, NULL, NULL, NULL)) {
    Napi::TypeError::New(env, StringFormat("BASS ERROR %d in Init",  BASS_ErrorGetCode())).ThrowAsJavaScriptException();
    return env.Null();
  }
/*
  // Load the Plugins
  if (!BASS_PluginLoad("basswma.dll", 0) && BASS_ErrorGetCode() != BASS_ERROR_ALREADY) {
    Napi::TypeError::New(env, string_format("BASS ERROR %d in PluginLoad WMA", BASS_ErrorGetCode())).ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!BASS_PluginLoad("bassflac.dll", 0) && BASS_ErrorGetCode() != BASS_ERROR_ALREADY) {
    Napi::TypeError::New(env, string_format("BASS ERROR %d in PluginLoad FLAC", BASS_ErrorGetCode())).ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!BASS_PluginLoad("bassmidi.dll", 0) && BASS_ErrorGetCode() != BASS_ERROR_ALREADY) {
    Napi::TypeError::New(env, string_format("BASS ERROR %d in PluginLoad MIDI", BASS_ErrorGetCode())).ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!BASS_PluginLoad("bass_aac.dll", 0) && BASS_ErrorGetCode() != BASS_ERROR_ALREADY) {
    Napi::TypeError::New(env, string_format("BASS ERROR %d in PluginLoad AAC", BASS_ErrorGetCode())).ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!BASS_PluginLoad("bass_ac3.dll", 0) && BASS_ErrorGetCode() != BASS_ERROR_ALREADY) {
    Napi::TypeError::New(env, string_format("BASS ERROR %d in PluginLoad AC3", BASS_ErrorGetCode())).ThrowAsJavaScriptException();
    return env.Null();
  }
  if (!BASS_PluginLoad("bassopus.dll", 0) && BASS_ErrorGetCode() != BASS_ERROR_ALREADY) {
    Napi::TypeError::New(env, string_format("BASS ERROR %d in PluginLoad OPUS", BASS_ErrorGetCode())).ThrowAsJavaScriptException();
    return env.Null();
  }
*/
  return Napi::Boolean::New(env, true);
}

HSTREAM ConvertFileToMono(const std::string& strPath)
{
    HSTREAM decoder = BASS_StreamCreateFile(FALSE, strPath.c_str(), 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_MONO); // open file for decoding  | BASS_UNICODE
    if (!decoder)
      return 0;                                                                                           // failed
    DWORD            length = static_cast<DWORD>(BASS_ChannelGetLength(decoder, BASS_POS_BYTE));            // get the length
    void*            data = malloc(length);                                                                 // allocate buffer for decoded data
    BASS_CHANNELINFO ci;
    BASS_ChannelGetInfo(decoder, &ci);            // get sample format
    if (ci.chans > 1)                             // not mono, downmix...
    {
        HSTREAM mixer = BASS_Mixer_StreamCreate(ci.freq, 1, BASS_STREAM_DECODE | BASS_MIXER_END);            // create mono mixer
        BASS_Mixer_StreamAddChannel(
            mixer, decoder, BASS_MIXER_DOWNMIX | BASS_MIXER_NORAMPIN | BASS_STREAM_AUTOFREE);            // plug-in the decoder (auto-free with the mixer)
        decoder = mixer;                                                                                 // decode from the mixer
    }
    length = BASS_ChannelGetData(decoder, data, length);                                                    // decode data
    BASS_StreamFree(decoder);                                                                               // free the decoder/mixer
    HSTREAM stream = BASS_StreamCreate(ci.freq, 1, BASS_STREAM_AUTOFREE, STREAMPROC_PUSH, NULL);            // create stream
    BASS_StreamPutData(stream, data, length);                                                               // set the stream data
    free(data);                                                                                             // free the buffer
    return stream;
}

Napi::Value play(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  JS_ARGS(1);
  JS_ARG_TYPE(0, String);
  JS_STR_ARG(0, strPath);

  long lCreateFlags = BASS_MUSIC_PRESCAN | BASS_STREAM_DECODE;
  BOOL b3D = FALSE;
  BOOL bLoop = FALSE;

  //char file[MAX_PATH] = "";
  //strPath.copy(file, strPath.size());

  HSTREAM pSound = BASS_StreamCreateFile(FALSE, strPath.c_str(), 0, 0, lCreateFlags ); // | BASS_UNICODE

  if (!pSound)
      pSound = BASS_MusicLoad(FALSE, strPath.c_str(), 0, 0, BASS_MUSIC_RAMP | BASS_MUSIC_PRESCAN | BASS_STREAM_DECODE, 0); // Try again | BASS_UNICODE

  if (!pSound && b3D)
      pSound = ConvertFileToMono(strPath.c_str()); // Last try if 3D

  if (!pSound)
  {
    Napi::TypeError::New(env, StringFormat("BASS ERROR %d in LoadMedia  path:%s  3d:%d  loop:%d", BASS_ErrorGetCode(), strPath.c_str(), b3D, bLoop)).ThrowAsJavaScriptException();
    return env.Null();
  }

  return Napi::Number::New(env, 0);
}


Napi::Value loadPlugin(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  JS_ARGS(1);
  JS_ARG_TYPE(0, String);
  JS_STR_ARG(0, pluginFile);
  if (!BASS_PluginLoad(pluginFile.c_str(), 0) && BASS_ErrorGetCode() != BASS_ERROR_ALREADY) {
    Napi::TypeError::New(env, StringFormat("Sound ERROR %d in loadPlugin %s", BASS_ErrorGetCode(), pluginFile.c_str())).ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  return Napi::Boolean::New(env, true);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  JS_SET_METHOD(init);
  JS_SET_METHOD(loadPlugin);
  return NativeSound::Init(env, exports);
}

NODE_API_MODULE(hello, Init)

}
