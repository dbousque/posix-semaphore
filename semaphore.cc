#include "semaphore.h"
#include <assert.h>

napi_ref Semaphore::constructor;

Semaphore::Semaphore(napi_env env, char buf[], size_t buf_len) : env_(nullptr), wrapper_(nullptr)
{
  napi_status status;
  size_t      i;

  i = 0;
  while (i < buf_len)
  {
    this->sem_name[i] = buf[i];
    i++;
  }
  this->semaphore = sem_open(sem_name, O_CREAT, 0644, 1);
  if(this->semaphore == SEM_FAILED)
  {
    status = napi_throw_error(env, "could not create semaphore : sem_open failed");
    assert(status == napi_ok);
    return ;
  }
  this->locked = 0;
  this->closed = 0;
}

Semaphore::~Semaphore()
{
  napi_delete_reference(env_, wrapper_);
}

void Semaphore::Destructor(napi_env env, void* nativeObject, void* /*finalize_hint*/) {
  reinterpret_cast<Semaphore*>(nativeObject)->~Semaphore();
}

#define DECLARE_NAPI_METHOD(name, func)                          \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

void Semaphore::Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor properties[] = {
      DECLARE_NAPI_METHOD("acquire", Acquire),
      DECLARE_NAPI_METHOD("release", Release),
      DECLARE_NAPI_METHOD("close", Close),
  };

  napi_value cons;
  status = napi_define_class(env, "Semaphore", New, nullptr, 3, properties, &cons);
  assert(status == napi_ok);

  status = napi_create_reference(env, cons, 1, &constructor);
  assert(status == napi_ok);

  status = napi_set_named_property(env, exports, "Semaphore", cons);
  assert(status == napi_ok);
}

napi_value Semaphore::New(napi_env env, napi_callback_info info) {
  napi_status status;

  bool is_constructor;
  status = napi_is_construct_call(env, info, &is_constructor);
  assert(status == napi_ok);

  if (is_constructor) {
    // Invoked as constructor: `new Semaphore(...)`
    size_t argc = 1;
    napi_value args[1];
    napi_value jsthis;
    status = napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr);
    assert(status == napi_ok);

    napi_valuetype valuetype;
    status = napi_typeof(env, args[0], &valuetype);
    assert(status == napi_ok);

    if (valuetype != napi_string) {
      status = napi_throw_type_error(env, "Semaphore() expects a string as argument");
      assert(status == napi_ok);
      return nullptr;
    }
    char buf[256];
    size_t  res_len;
    status = napi_get_value_string_utf8(env, args[0], buf, 256, &res_len);
    assert(status == napi_ok);

    if (res_len >= 255) {
      status = napi_throw_error(env, "Semaphore() : argument must be less than 255 bytes long");
      assert(status == napi_ok);
      return nullptr;
    }

    Semaphore* obj = new Semaphore(env, buf, res_len);

    obj->env_ = env;
    status = napi_wrap(env,
                       jsthis,
                       reinterpret_cast<void*>(obj),
                       Semaphore::Destructor,
                       nullptr,  // finalize_hint
                       &obj->wrapper_);
    assert(status == napi_ok);

    return jsthis;
  } else {
    // Invoked as plain function `Semaphore(...)`, turn into construct call.
    size_t argc_ = 1;
    napi_value args[1];
    status = napi_get_cb_info(env, info, &argc_, args, nullptr, nullptr);
    assert(status == napi_ok);

    const size_t argc = 1;
    napi_value argv[argc] = {args[0]};

    napi_value cons;
    status = napi_get_reference_value(env, constructor, &cons);
    assert(status == napi_ok);

    napi_value instance;
    status = napi_new_instance(env, cons, argc, argv, &instance);
    assert(status == napi_ok);

    return instance;
  }
}

napi_value Semaphore::Acquire(napi_env env, napi_callback_info info)
{
  napi_status status;
  napi_value  jsthis;
  Semaphore   *obj;

  status = napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr);
  assert(status == napi_ok);

  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  assert(status == napi_ok);

  if (obj->closed)
  {
    status = napi_throw_error(env, "trying to do operation over semaphore, but already closed");
    assert(status == napi_ok);
    return nullptr;
  }

  if (obj->locked)
  {
    printf("trying to acquire semaphore, but already acquired\n");
    status = napi_throw_error(env, "trying to acquire semaphore, but already acquired");
    assert(status == napi_ok);
    return nullptr;
  }
  if (sem_wait(obj->semaphore) == -1)
  {
    status = napi_throw_error(env, "could not acquire semaphore, sem_wait failed");
    assert(status == napi_ok);
    return nullptr;
  }
  obj->locked = 1;
  return nullptr;
}

napi_value Semaphore::Release(napi_env env, napi_callback_info info)
{
  napi_status status;
  napi_value  jsthis;
  Semaphore   *obj;

  status = napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr);
  assert(status == napi_ok);

  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  assert(status == napi_ok);

  if (obj->closed)
  {
    status = napi_throw_error(env, "trying to do operation over semaphore, but already closed");
    assert(status == napi_ok);
    return nullptr;
  }

  if (!obj->locked)
  {
    printf("trying to release semaphore, but already released\n");
    status = napi_throw_error(env, "trying to release semaphore, but already released");
    assert(status == napi_ok);
    return nullptr;
  }
  if (sem_post(obj->semaphore) == -1)
  {
    status = napi_throw_error(env, "could not release semaphore, sem_post failed");
    assert(status == napi_ok);
    return nullptr;
  }
  obj->locked = 0;
  return nullptr;
}

napi_value Semaphore::Close(napi_env env, napi_callback_info info)
{
  napi_status status;
  napi_value  jsthis;
  Semaphore   *obj;

  status = napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr);
  assert(status == napi_ok);

  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  assert(status == napi_ok);

  if (obj->closed)
  {
    status = napi_throw_error(env, "trying to do operation over semaphore, but already closed");
    assert(status == napi_ok);
    return nullptr;
  }
  if (obj->locked)
  {
    Semaphore::Release(env, info);
  }

  sem_close(obj->semaphore);
  sem_unlink(obj->sem_name);
  obj->closed = 1;
  return nullptr;
}
