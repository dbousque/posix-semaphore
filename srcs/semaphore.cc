#include "semaphore.h"
#include "typeinfo"

Nan::Persistent<v8::Function> Semaphore::constructor;

Semaphore::Semaphore(char buf[], size_t buf_len, bool strict, bool debug, bool silent, bool retry_on_eintr, unsigned int value /*= 1*/)
{
  strcpy(this->sem_name, buf);
  this->semaphore = sem_open(this->sem_name, O_CREAT, 0644, value);
  if (this->semaphore == SEM_FAILED)
  {
    this->closed = 1;
    Nan::ThrowError("Could not create semaphore: sem_open failed");
    return ;
  }
  this->locked = false;
  this->closed = false;
  this->strict = strict;
  this->debug = debug;
  this->silent = silent;
  this->retry_on_eintr = retry_on_eintr;
}

Semaphore::~Semaphore()
{
}

void Semaphore::Init(v8::Local<v8::Object> exports)
{
#if NODE_MAJOR_VERSION >= 18
  v8::Local<v8::Context> context = exports->GetCreationContextChecked();
#else
  v8::Local<v8::Context> context = exports->CreationContext();
#endif

  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Semaphore").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "acquire", Acquire);
  Nan::SetPrototypeMethod(tpl, "release", Release);
  Nan::SetPrototypeMethod(tpl, "close", Close);

  constructor.Reset(tpl->GetFunction(context).ToLocalChecked());
  exports->Set(
    context,
    Nan::New("Semaphore").ToLocalChecked(),
    tpl->GetFunction(context).ToLocalChecked()
  );
}

void Semaphore::New(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  bool strict;
  bool debug;
  bool silent;
  bool retry_on_eintr;
  char buf[SEMSIZE];
  unsigned int value;

  if (!info.IsConstructCall())
    return Nan::ThrowError("Must call Semaphore() with new");
  if ((info.Length() < 5) || (info.Length() > 6))
    return Nan::ThrowError("Semaphore() expects 5 or 6 arguments");
  if (!info[0]->IsString())
    return Nan::ThrowError("Semaphore() expects a string as first argument");
  if (!info[1]->IsBoolean())
    return Nan::ThrowError("Semaphore() expects a boolean as second argument");
  if (!info[2]->IsBoolean())
    return Nan::ThrowError("Semaphore() expects a boolean as third argument");
  if (!info[3]->IsBoolean())
    return Nan::ThrowError("Semaphore() expects a boolean as fourth argument");
  if (!info[4]->IsBoolean())
    return Nan::ThrowError("Semaphore() expects a boolean as fifth argument");
  if (!info[5]->IsUndefined() && !info[5]->IsUint32())
    return Nan::ThrowError("Semaphore() expects an integer as sixth argument");
  strict = Nan::To<bool>(info[1]).FromJust();
  debug = Nan::To<bool>(info[2]).FromJust();
  silent = Nan::To<bool>(info[3]).FromJust();
  retry_on_eintr = Nan::To<bool>(info[4]).FromJust();
  value = !info[5]->IsUndefined()? info[5]->IntegerValue(context).FromJust(): 1;
  v8::String::Utf8Value v8str(isolate, info[0]);
  std::string str(*v8str);

  size_t str_len;
  str_len = str.length();
  strncpy(buf, str.c_str(), str_len);
  buf[str_len] = '\0';

  if (str_len >= SEMSIZE - 1 || str_len <= 0)
    return Nan::ThrowError("Semaphore() : first argument's length must be < 255 && > 0");

  Semaphore* obj = new Semaphore(buf, str_len, strict, debug, silent, retry_on_eintr, value);
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

void Semaphore::Acquire(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
  int r;
  Semaphore* obj = ObjectWrap::Unwrap<Semaphore>(info.Holder());

  if (obj->strict && obj->closed)
    return Nan::ThrowError("trying to do operation over semaphore, but already closed");
  if (obj->strict && obj->locked)
    return Nan::ThrowError("trying to acquire semaphore, but already acquired");
  while ((r = sem_wait(obj->semaphore)) == -1 && errno == EINTR && obj->retry_on_eintr)
  {
    if (obj->debug)
      printf("[posix-semaphore] Got EINTR while calling sem_wait, retrying...\n");
  }
  // error and not EINTR
  if (r == -1)
  {
    if (obj->debug || !obj->silent)
    {
      if (errno == EINTR)
      {
        printf("[posix-semaphore] Got EINTR while calling sem_wait, expected on SIGINT (CTRL-C). Not retrying because the 'retryOnEintr' option is false\n");
      }
      else
      {
        printf("[posix-semaphore] sem_wait failed, printing errno message ('man sem_wait' for more details on possible errors) : \n");
        perror("[posix-semaphore] ");
      }
    }
    if (errno != EINTR)
      Nan::ThrowError("could not acquire semaphore, sem_wait failed");
    return ;
  }
  obj->locked = true;
}

void Semaphore::Release(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
  Semaphore* obj = ObjectWrap::Unwrap<Semaphore>(info.Holder());

  if (obj->strict && obj->closed)
    return Nan::ThrowError("trying to do operation over semaphore, but already closed");
  if (obj->strict && !obj->locked)
    return Nan::ThrowError("trying to release semaphore, but already released");
  if (sem_post(obj->semaphore) == -1)
  {
    if (obj->debug || !obj->silent)
    {
      printf("[posix-semaphore] sem_post failed, printing errno message ('man sem_post' for more details on possible errors): \n");
      perror("[posix-semaphore] ");
    }
    return Nan::ThrowError("could not release semaphore, sem_post failed");
  }
  obj->locked = false;
}

void Semaphore::Close(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
  Semaphore* obj = ObjectWrap::Unwrap<Semaphore>(info.Holder());

  if (obj->strict && obj->closed)
    return Nan::ThrowError("trying to close semaphore, but already closed");
  if (obj->debug)
    printf("[posix-semaphore] Closing semaphore\n");
  obj->closed = true;
  sem_close(obj->semaphore);
  sem_unlink(obj->sem_name);
}
