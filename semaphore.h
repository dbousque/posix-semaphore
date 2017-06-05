#ifndef TEST_ADDONS_NAPI_6_OBJECT_WRAP_SEMAPHORE_H_
#define TEST_ADDONS_NAPI_6_OBJECT_WRAP_SEMAPHORE_H_

#include <node_api.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>

class Semaphore {
 public:
  static void Init(napi_env env, napi_value exports);
  static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);

 private:
  explicit Semaphore(napi_env env, char buf[], size_t buf_len);
  ~Semaphore();

  static napi_value New(napi_env env, napi_callback_info info);
  static napi_value Acquire(napi_env env, napi_callback_info info);
  static napi_value Release(napi_env env, napi_callback_info info);
  static napi_value Close(napi_env env, napi_callback_info info);
  static napi_ref constructor;
  napi_env env_;
  napi_ref wrapper_;

  char  sem_name[256];
  sem_t *semaphore;
  char  locked;
  char  closed;
};

#endif  // TEST_ADDONS_NAPI_6_OBJECT_WRAP_SEMAPHORE_H_
