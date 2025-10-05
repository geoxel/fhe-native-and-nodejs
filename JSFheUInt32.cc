#include "JSFheUInt32.h"
#include <napi.h>
#include "tfhe.h"

JSFheUInt32::JSFheUInt32(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<JSFheUInt32>(info)
{
}

JSFheUInt32::~JSFheUInt32()
{
  Destroy();
}

void JSFheUInt32::Destroy()
{
  if (_fhe != nullptr)
  {
    fhe_uint32_destroy(_fhe);
    _fhe = nullptr;
  }
}

void JSFheUInt32::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function func = DefineClass(env, "FheUInt32", {});

  Napi::FunctionReference *constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  env.SetInstanceData(constructor); // NOTE: this assumes only 1 class is
                                    // exported for multiple exported classes,
                                    // need a struct or other mechanism

  exports.Set("FheUInt8", func);
}

Napi::Object JSFheUInt32::NewInstance(Napi::Env env, Napi::Value arg)
{
  Napi::Object obj = env.GetInstanceData<Napi::FunctionReference>()->New({arg});
  return obj;
}
