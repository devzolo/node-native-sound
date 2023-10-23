#ifndef COMMON_HH
#define COMMON_HH

#define JS____________________________TODO(name) puts("TODO: GL."#name)
#define JS_CONSTANT(name) exports.Set(Napi::String::New(env, #name), Napi::Number::New(env, #name))
#define JS_SET_METHOD(name) exports.Set(Napi::String::New(env, #name), Napi::Function::New(env, name));
#define JS_ARGS(argc) \
if (info.Length() < argc) { \
  Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException(); \
  return env.Null(); \
}

#define JS_ARG_TYPE(pos, type)  if (!info[pos].Is##type()) { \
  Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException(); \
  return env.Null(); \
}

#define JS_INT_ARG(pos, name) int32_t name = info[pos].As<Napi::Number>().Int32Value();
#define JS_LONG_ARG(pos, name) int64_t name = info[pos].As<Napi::Number>().Int64Value();
#define JS_UINT_ARG(pos, name) uint32_t name = info[pos].As<Napi::Number>().Uint32Value();
#define JS_STR_ARG(pos, name) std::string name = info[pos].As<Napi::String>().Utf8Value();
#define JS_DOUBLE_ARG(pos, name) double_t name = info[pos].As<Napi::Number>().DoubleValue();
#define JS_FLOAT_ARG(pos, name) float_t name = info[pos].As<Napi::Number>().FloatValue();
#define JS_PVOID_ARG(pos, name) void* name = (void*)info[pos].As<Napi::Number>().Int64Value();

inline Napi::Float32Array float2js(Napi::Env &env, float *data, uint32_t size) {
  auto arrView = Napi::ArrayBuffer::New(
      env,
      data,
      size * sizeof(float),
      [](Napi::Env /*env*/, void *finalizeData) {
          free(finalizeData);
      }
  );

  return Napi::Float32Array::New(env, size, arrView, 0);
}

inline Napi::Value float2external(Napi::Env &env, float *data) {
  return Napi::External<float>::New(env, data);
}

inline float *external2float(Napi::Value val) {
  return val.As<Napi::External<float>>().Data();
}

#endif //COMMON_HH
