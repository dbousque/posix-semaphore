#include "semaphore.h"

Nan::Persistent<v8::Function> Semaphore::constructor;

Semaphore::Semaphore(char buf[], size_t buf_len, bool strict, bool debug, bool silent, bool retry_on_eintr)
{
  size_t  i;

  i = 0;
  while (i < buf_len)
  {
    this->sem_name[i] = buf[i];
    i++;
  }
  this->semaphore = sem_open(this->sem_name, O_CREAT, 0644, 1);
  if(this->semaphore == SEM_FAILED)
  {
    this->closed = 1;
    Nan::ThrowError("could not create semaphore : sem_open failed");
    return ;
  }
  if (debug)
    printf("[posix-semaphore] Opened semaphore\n");
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
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Semaphore").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "acquire", Acquire);
  Nan::SetPrototypeMethod(tpl, "release", Release);
  Nan::SetPrototypeMethod(tpl, "close", Close);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("Semaphore").ToLocalChecked(), tpl->GetFunction());
}

void Semaphore::New(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
  bool    strict;
  bool    debug;
  bool    silent;
  bool    retry_on_eintr;
  char    *buf;

  if (!info.IsConstructCall())
    return Nan::ThrowError("Must call Semaphore() with new");
  if (info.Length() != 5)
    return Nan::ThrowError("Semaphore() expects 5 arguments");
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
  strict = info[1]->BooleanValue();
  debug = info[2]->BooleanValue();
  silent = info[3]->BooleanValue();
  retry_on_eintr = info[4]->BooleanValue();
  v8::String::Utf8Value str(info[0]->ToString());
  if (str.length() >= 255 || str.length() <= 0)
    return Nan::ThrowError("Semaphore() : first argument's length must be < 255 && > 0");
  buf = (char*)(*str);
  Semaphore* obj = new Semaphore(buf, str.length(), strict, debug, silent, retry_on_eintr);
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

void Semaphore::Acquire(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
  int        r;
  Semaphore* obj = ObjectWrap::Unwrap<Semaphore>(info.Holder());

  /*if (!obj->strict && (obj->closed || obj->locked))
  {
    if (obj->debug)
      printf("[posix-semaphore] 'acquire' called when semaphore was already acquired or closed, but strict mode deactivated, so not failing\n");
    return ;
  }*/
  if (obj->strict && obj->closed)
    return Nan::ThrowError("trying to do operation over semaphore, but already closed");
  if (obj->strict && obj->locked)
    return Nan::ThrowError("trying to acquire semaphore, but already acquired");
  if (obj->strict && obj->debug)
    printf("[posix-semaphore] Before sem_wait\n");
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
  if (obj->debug)
    printf("[posix-semaphore] After sem_wait\n");
}

void Semaphore::Release(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
  Semaphore* obj = ObjectWrap::Unwrap<Semaphore>(info.Holder());
  
  /*if (!obj->strict && (obj->closed || !obj->locked))
  {
    if (obj->debug)
      printf("[posix-semaphore] 'release' called when semaphore was already released or closed, but strict mode deactivated, so not failing\n");
    return ;
  }*/
  if (obj->strict && obj->closed)
    return Nan::ThrowError("trying to do operation over semaphore, but already closed");
  if (obj->strict && !obj->locked)
    return Nan::ThrowError("trying to release semaphore, but already released");
  if (obj->debug)
    printf("[posix-semaphore] Before sem_post\n");
  if (sem_post(obj->semaphore) == -1)
  {
    if (obj->debug || !obj->silent)
    {
      printf("[posix-semaphore] sem_post failed, printing errno message ('man sem_post' for more details on possible errors) : \n");
      perror("[posix-semaphore] ");
    }
    return Nan::ThrowError("could not release semaphore, sem_post failed");
  }
  obj->locked = false;
  if (obj->debug)
    printf("[posix-semaphore] After sem_post\n");
}

void Semaphore::Close(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
  Semaphore* obj = ObjectWrap::Unwrap<Semaphore>(info.Holder());

  /*if (!obj->strict && obj->closed)
  {
    if (obj->debug)
      printf("[posix-semaphore] 'close' called when semaphore was already closed, but strict mode deactivated, so not failing\n");
    return ;
  }*/
  if (obj->strict && obj->closed)
    return Nan::ThrowError("trying to close semaphore, but already closed");
  if (obj->debug)
    printf("[posix-semaphore] Closing semaphore\n");
  obj->closed = true;
  sem_close(obj->semaphore);
  sem_unlink(obj->sem_name);
}
