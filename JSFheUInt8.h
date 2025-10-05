#ifndef JSFheUInt8_H
#define JSFheUInt8_H

#include <napi.h>

struct FheUint8;

class JSFheUInt8 : public Napi::ObjectWrap<JSFheUInt8>
{
public:
  static void Init(Napi::Env env, Napi::Object exports);
  static Napi::Object NewInstance(Napi::Env env, Napi::Value arg);

  void SetFhe(FheUint8 *fhe)
  {
    _fhe = fhe;
  }

  FheUint8 *GetFhe() const
  {
    return _fhe;
  }

  JSFheUInt8(const Napi::CallbackInfo &info);
  virtual ~JSFheUInt8();
  void Destroy();

private:
  FheUint8 *_fhe;
};

#endif
