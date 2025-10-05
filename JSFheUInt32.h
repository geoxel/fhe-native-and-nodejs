#ifndef JSFheUInt32_H
#define JSFheUInt32_H

#include <napi.h>

struct FheUint32;

class JSFheUInt32 : public Napi::ObjectWrap<JSFheUInt32>
{
public:
  static void Init(Napi::Env env, Napi::Object exports);
  static Napi::Object NewInstance(Napi::Env env, Napi::Value arg);

  void SetFhe(FheUint32 *fhe)
  {
    _fhe = fhe;
  }

  FheUint32 *GetFhe() const
  {
    return _fhe;
  }

  JSFheUInt32(const Napi::CallbackInfo &info);
  virtual ~JSFheUInt32();
  void Destroy();

private:
  FheUint32 *_fhe;
};

#endif
