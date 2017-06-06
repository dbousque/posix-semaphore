#include "semaphore.h"

Nan::Persistent<v8::Function> Semaphore::constructor;

Semaphore::Semaphore(char buf[], size_t buf_len, bool strict)
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
  this->locked = false;
  this->closed = false;
  this->strict = strict;
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
  char    *buf;

  strict = false;
  if (!info.IsConstructCall())
    return Nan::ThrowError("Must call Semaphore() with new");
  if (!info[0]->IsString())
    return Nan::ThrowError("Semaphore() expects a string as first argument");
  if (info.Length() > 1)
  {
    if (!info[1]->IsBoolean())
      return Nan::ThrowError("Semaphore() expects a boolean as second argument, if provided");
    strict = info[1]->BooleanValue();
  }
  v8::String::Utf8Value str(info[0]->ToString());
  if (str.length() >= 255)
    return Nan::ThrowError("Semaphore() : first argument's length must be < 255");
  buf = (char*)(*str);
  Semaphore* obj = new Semaphore(buf, str.length(), strict);
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

void Semaphore::Acquire(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
  Semaphore* obj = ObjectWrap::Unwrap<Semaphore>(info.Holder());
  (void)obj;
  //info.GetReturnValue().Set(Nan::New(obj->value_));
}

void Semaphore::Release(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
  Semaphore* obj = ObjectWrap::Unwrap<Semaphore>(info.Holder());
  (void)obj;
  //info.GetReturnValue().Set(Nan::New(obj->value_));
}

void Semaphore::Close(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
  Semaphore* obj = ObjectWrap::Unwrap<Semaphore>(info.Holder());
  (void)obj;
  //info.GetReturnValue().Set(Nan::New(obj->value_));
}
