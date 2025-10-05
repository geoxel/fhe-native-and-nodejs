#include <napi.h>
#include "tfhe.h"
#include "JSFheUInt8.h"
#include "JSFheUInt32.h"

#define assert_m(condition)                                                                              \
  do                                                                                                     \
  {                                                                                                      \
    if (!(condition))                                                                                    \
    {                                                                                                    \
      fprintf(stderr, "%s::%d::%s: condition `%s` failed.\n", __FILE__, __LINE__, __func__, #condition); \
      abort();                                                                                           \
    }                                                                                                    \
  } while (0)

static ClientKey *g_client_key = NULL;
static ServerKey *g_server_key = NULL;
static PublicKey *g_public_key = NULL;

static Napi::Value GenerateKeys(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (g_client_key != NULL)
    return env.Null();

  ConfigBuilder *builder = NULL;
  Config *config = NULL;

  int ok = 0;
  ok = config_builder_default(&builder);
  assert_m(ok == 0);
  ok = config_builder_build(builder, &config);
  assert_m(ok == 0);

  ok = generate_keys(config, &g_client_key, &g_server_key);
  assert_m(ok == 0);

  // printf("public_key_new...\n");
  // ok = public_key_new(g_client_key, &g_public_key);
  // assert_m(ok == 0);

  ok = set_server_key(g_server_key);
  assert_m(ok == 0);

  return env.Null();
}

static Napi::Value DestroyKeys(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (g_client_key != NULL)
  {
    client_key_destroy(g_client_key);
    g_client_key = NULL;
  }

  if (g_public_key != NULL)
  {
    public_key_destroy(g_public_key);
    g_public_key = NULL;
  }

  if (g_public_key != NULL)
  {
    server_key_destroy(g_server_key);
    g_server_key = NULL;
  }

  return env.Null();
}

static Napi::Value SerializeClientKey(const Napi::CallbackInfo &info)
{
  DynamicBuffer cks_buffer = {.pointer = NULL, .length = 0, .destructor = NULL};

  Napi::Env env = info.Env();

  int ok = client_key_serialize(g_client_key, &cks_buffer);
  if (ok != 0)
    return env.Null();

  Napi::ArrayBuffer buffer = Napi::ArrayBuffer::New(env, cks_buffer.length);
  std::memcpy(buffer.Data(), cks_buffer.pointer, cks_buffer.length);
  Napi::Uint8Array arr = Napi::Uint8Array::New(env, cks_buffer.length, buffer, 0);

  if (cks_buffer.pointer != NULL)
    destroy_dynamic_buffer(&cks_buffer);

  return arr;
}

// UInt8 functions

static Napi::Value EncryptUInt8(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  uint8_t value = (uint8_t)info[0].As<Napi::Number>().DoubleValue();

  FheUint8 *fhe = NULL;
  int ok = fhe_uint8_try_encrypt_with_client_key_u8(value, g_client_key, &fhe);
  if (ok != 0)
    return env.Null();

  Napi::Object obj = JSFheUInt8::NewInstance(env, env.Null());

  // TODO: should instead replace the env.Null() above to set the result.
  JSFheUInt8 *jsfhe = Napi::ObjectWrap<JSFheUInt8>::Unwrap(obj);
  jsfhe->SetFhe(fhe);

  return obj;
}

static Napi::Value DecryptUInt8(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  JSFheUInt8 *obj = Napi::ObjectWrap<JSFheUInt8>::Unwrap(info[0].As<Napi::Object>());
  FheUint8 *fhe = obj->GetFhe();

  uint8_t value;
  int ok = fhe_uint8_decrypt(fhe, g_client_key, &value);
  if (ok != 0)
    return env.Null();

  return Napi::Number::New(env, value);
}

static Napi::Value SerializetUInt8(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  JSFheUInt8 *obj = Napi::ObjectWrap<JSFheUInt8>::Unwrap(info[0].As<Napi::Object>());
  FheUint8 *fhe = obj->GetFhe();

  DynamicBuffer result = {.pointer = NULL, .length = 0, .destructor = NULL};
  int ok = fhe_uint8_serialize(fhe, &result);
  if (ok != 0)
    return env.Null();

  Napi::ArrayBuffer buffer = Napi::ArrayBuffer::New(env, result.length);
  std::memcpy(buffer.Data(), result.pointer, result.length);
  Napi::Uint8Array arr = Napi::Uint8Array::New(env, result.length, buffer, 0);

  if (result.pointer != NULL)
    destroy_dynamic_buffer(&result);

  return arr;
}

static Napi::Value DeserializetUInt8(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  // skipping all the arguments checks, etc

  Napi::TypedArray typedArray = info[0].As<Napi::TypedArray>();
  Napi::Uint8Array arr = typedArray.As<Napi::Uint8Array>();

  DynamicBufferView input = {.pointer = arr.Data(), .length = arr.ElementLength()};

  FheUint8 *result = NULL;
  int ok = fhe_uint8_deserialize(input, &result);
  if (ok != 0)
    return env.Null();

  Napi::Object obj = JSFheUInt8::NewInstance(env, env.Null());
  // TODO: should instead replace the env.Null() above to set the result.
  JSFheUInt8 *jsfhe = Napi::ObjectWrap<JSFheUInt8>::Unwrap(obj);
  jsfhe->SetFhe(result);

  return obj;
}

static Napi::Value AddUInt8(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  JSFheUInt8 *arg1 = Napi::ObjectWrap<JSFheUInt8>::Unwrap(info[0].As<Napi::Object>());
  JSFheUInt8 *arg2 = Napi::ObjectWrap<JSFheUInt8>::Unwrap(info[1].As<Napi::Object>());

  FheUint8 *fhe1 = arg1->GetFhe();
  FheUint8 *fhe2 = arg2->GetFhe();
  FheUint8 *result = NULL;

  int ok = fhe_uint8_add(fhe1, fhe2, &result);
  if (ok != 0)
    return env.Null();

  Napi::Object obj = JSFheUInt8::NewInstance(env, env.Null());

  // TODO: should instead replace the env.Null() above to set the result.
  JSFheUInt8 *jsfhe = Napi::ObjectWrap<JSFheUInt8>::Unwrap(obj);
  jsfhe->SetFhe(result);

  return obj;
}

static Napi::Value DestroyUInt8(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  JSFheUInt8 *arg = Napi::ObjectWrap<JSFheUInt8>::Unwrap(info[0].As<Napi::Object>());
  arg->Destroy();

  return env.Null();
}

// UInt32 functions

static Napi::Value EncryptUInt32(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  uint32_t value = (uint32_t)info[0].As<Napi::Number>().DoubleValue();

  FheUint32 *fhe = NULL;
  int ok = fhe_uint32_try_encrypt_with_client_key_u32(value, g_client_key, &fhe);
  if (ok != 0)
    return env.Null();

  Napi::Object obj = JSFheUInt8::NewInstance(env, env.Null());

  // TODO: should instead replace the env.Null() above to set the result.
  JSFheUInt32 *jsfhe = Napi::ObjectWrap<JSFheUInt32>::Unwrap(obj);
  jsfhe->SetFhe(fhe);

  return obj;
}

static Napi::Value DecryptUInt32(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  JSFheUInt32 *obj = Napi::ObjectWrap<JSFheUInt32>::Unwrap(info[0].As<Napi::Object>());
  FheUint32 *fhe = obj->GetFhe();

  uint32_t value;
  int ok = fhe_uint32_decrypt(fhe, g_client_key, &value);
  if (ok != 0)
    return env.Null();

  return Napi::Number::New(env, value);
}

static Napi::Value SerializetUInt32(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  JSFheUInt32 *obj = Napi::ObjectWrap<JSFheUInt32>::Unwrap(info[0].As<Napi::Object>());
  FheUint32 *fhe = obj->GetFhe();

  DynamicBuffer result = {.pointer = NULL, .length = 0, .destructor = NULL};
  int ok = fhe_uint32_serialize(fhe, &result);
  if (ok != 0)
    return env.Null();

  Napi::ArrayBuffer buffer = Napi::ArrayBuffer::New(env, result.length);
  std::memcpy(buffer.Data(), result.pointer, result.length);
  Napi::Uint8Array arr = Napi::Uint8Array::New(env, result.length, buffer, 0);

  if (result.pointer != NULL)
    destroy_dynamic_buffer(&result);

  return arr;
}

static Napi::Value DeserializetUInt32(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  // skipping all the arguments checks, etc

  Napi::TypedArray typedArray = info[0].As<Napi::TypedArray>();
  Napi::Uint8Array arr = typedArray.As<Napi::Uint8Array>();

  DynamicBufferView input = {.pointer = arr.Data(), .length = arr.ElementLength()};

  FheUint32 *result = NULL;
  int ok = fhe_uint32_deserialize(input, &result);
  if (ok != 0)
    return env.Null();

  Napi::Object obj = JSFheUInt32::NewInstance(env, env.Null());
  // TODO: should instead replace the env.Null() above to set the result.
  JSFheUInt32 *jsfhe = Napi::ObjectWrap<JSFheUInt32>::Unwrap(obj);
  jsfhe->SetFhe(result);

  return obj;
}

static Napi::Value AddUInt32(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  JSFheUInt32 *arg1 = Napi::ObjectWrap<JSFheUInt32>::Unwrap(info[0].As<Napi::Object>());
  JSFheUInt32 *arg2 = Napi::ObjectWrap<JSFheUInt32>::Unwrap(info[1].As<Napi::Object>());

  FheUint32 *fhe1 = arg1->GetFhe();
  FheUint32 *fhe2 = arg2->GetFhe();
  FheUint32 *result = NULL;

  int ok = fhe_uint32_add(fhe1, fhe2, &result);
  if (ok != 0)
    return env.Null();

  Napi::Object obj = JSFheUInt32::NewInstance(env, env.Null());

  // TODO: should instead replace the env.Null() above to set the result.
  JSFheUInt32 *jsfhe = Napi::ObjectWrap<JSFheUInt32>::Unwrap(obj);
  jsfhe->SetFhe(result);

  return obj;
}

static Napi::Value DestroyUInt32(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  JSFheUInt32 *arg = Napi::ObjectWrap<JSFheUInt32>::Unwrap(info[0].As<Napi::Object>());
  arg->Destroy();

  return env.Null();
}

// Main

static Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  JSFheUInt8::Init(env, exports);

  exports.Set("GenerateKeys", Napi::Function::New(env, GenerateKeys));
  exports.Set("DestroyKeys", Napi::Function::New(env, DestroyKeys));
  exports.Set("SerializeClientKey", Napi::Function::New(env, SerializeClientKey));

  exports.Set("EncryptUInt8", Napi::Function::New(env, EncryptUInt8));
  exports.Set("DecryptUInt8", Napi::Function::New(env, DecryptUInt8));
  exports.Set("SerializetUInt8", Napi::Function::New(env, SerializetUInt8));
  exports.Set("DeserializetUInt8", Napi::Function::New(env, DeserializetUInt8));
  exports.Set("AddUInt8", Napi::Function::New(env, AddUInt8));
  exports.Set("DestroyUInt8", Napi::Function::New(env, DestroyUInt8));

  exports.Set("EncryptUInt32", Napi::Function::New(env, EncryptUInt32));
  exports.Set("DecryptUInt32", Napi::Function::New(env, DecryptUInt32));
  exports.Set("SerializetUInt32", Napi::Function::New(env, SerializetUInt32));
  exports.Set("DeserializetUInt32", Napi::Function::New(env, DeserializetUInt32));
  exports.Set("AddUInt32", Napi::Function::New(env, AddUInt32));
  exports.Set("DestroyUInt32", Napi::Function::New(env, DestroyUInt32));

  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
