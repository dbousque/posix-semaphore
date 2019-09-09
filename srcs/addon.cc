#include <nan.h>
#include "semaphore.h"

NODE_MODULE_INIT() {
    Semaphore::Init(exports);
}
