#include <nan.h>
#include "semaphore.h"

void InitAll(v8::Local<v8::Object> exports) {
  Semaphore::Init(exports);
}

NODE_MODULE(addon, InitAll)
