#include <napi.h>
#include "tfhe.h"
#include "JSFheUInt8.h"

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

Napi::Value GenerateKeys(const Napi::CallbackInfo &info)
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

  printf("generate_keys...\n");

  ok = generate_keys(config, &g_client_key, &g_server_key);
  assert_m(ok == 0);

  // printf("public_key_new...\n");
  // ok = public_key_new(g_client_key, &g_public_key);
  // assert_m(ok == 0);

  ok = set_server_key(g_server_key);
  assert_m(ok == 0);

  return env.Null();
}

Napi::Value DestroyKeys(const Napi::CallbackInfo &info)
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

Napi::Value SerializeClientKey(const Napi::CallbackInfo &info)
{
  DynamicBuffer cks_buffer = {.pointer = NULL, .length = 0, .destructor = NULL};

  int ok = client_key_serialize(g_client_key, &cks_buffer); // TODO: what about freeing cks_buffer?
  assert_m(ok == 0);

  Napi::Env env = info.Env();

  Napi::ArrayBuffer buffer = Napi::ArrayBuffer::New(env, cks_buffer.length);
  std::memcpy(buffer.Data(), cks_buffer.pointer, cks_buffer.length);
  Napi::Uint8Array arr = Napi::Uint8Array::New(env, cks_buffer.length, buffer, 0);

  return arr;
}

Napi::Value EncryptUInt8(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  uint8_t value = (uint8_t)info[0].As<Napi::Number>().DoubleValue();

  FheUint8 *fhe = NULL;
  int ok = fhe_uint8_try_encrypt_with_client_key_u8(value, g_client_key, &fhe);
  assert_m(ok == 0);

  Napi::Object obj = JSFheUInt8::NewInstance(env, env.Null());

  JSFheUInt8 *jsfhe = Napi::ObjectWrap<JSFheUInt8>::Unwrap(obj);
  jsfhe->SetFhe(fhe);

  return obj;
}

Napi::Value DecryptUInt8(const Napi::CallbackInfo &info)
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

Napi::Value AddUInt8(const Napi::CallbackInfo &info)
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

  JSFheUInt8 *jsfhe = Napi::ObjectWrap<JSFheUInt8>::Unwrap(obj);
  jsfhe->SetFhe(result);

  return obj;
}

static Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  JSFheUInt8::Init(env, exports);

  exports.Set("GenerateKeys", Napi::Function::New(env, GenerateKeys));
  exports.Set("DestroyKeys", Napi::Function::New(env, DestroyKeys));
  exports.Set("SerializeClientKey", Napi::Function::New(env, SerializeClientKey));
  exports.Set("EncryptUInt8", Napi::Function::New(env, EncryptUInt8));
  exports.Set("DecryptUInt8", Napi::Function::New(env, DecryptUInt8));
  exports.Set("AddUInt8", Napi::Function::New(env, AddUInt8));

  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
