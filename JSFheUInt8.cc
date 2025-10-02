#include "JSFheUInt8.h"
#include <napi.h>
#include "tfhe.h"

JSFheUInt8::JSFheUInt8(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<JSFheUInt8>(info)
{
}

JSFheUInt8::~JSFheUInt8()
{
  if (_fhe != NULL)
    fhe_uint8_destroy(_fhe);
}

void JSFheUInt8::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function func = DefineClass(env, "FheUInt8", {});

  Napi::FunctionReference *constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  env.SetInstanceData(constructor); // NOTE: this assumes only 1 class is
                                    // exported for multiple exported classes,
                                    // need a struct or other mechanism

  exports.Set("FheUInt8", func);
}

Napi::Object JSFheUInt8::NewInstance(Napi::Env env, Napi::Value arg)
{
  Napi::Object obj = env.GetInstanceData<Napi::FunctionReference>()->New({arg});
  return obj;
}
