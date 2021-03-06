#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#define SEMSIZE 256

#include <nan.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>

class Semaphore : public Nan::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

 private:
  explicit Semaphore(char *buf, size_t buf_len, bool strict, bool debug, bool silent, bool retry_on_eintr, unsigned int value = 1);
  ~Semaphore();

  static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void Acquire(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void Release(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void Close(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static Nan::Persistent<v8::Function> constructor;

  sem_t	*semaphore;
  char	sem_name[SEMSIZE];
  bool	locked;
  bool	closed;
  bool  strict;
  bool  debug;
  bool  silent;
  bool  retry_on_eintr;
};

#endif
