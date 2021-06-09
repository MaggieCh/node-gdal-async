#ifndef __NODE_GDAL_ASYNC_WORKER_H__
#define __NODE_GDAL_ASYNC_WORKER_H__

#include <functional>
#include "nan-wrapper.h"
#include "gdal_common.hpp"

namespace node_gdal {

// This generates method definitions for 2 methods: sync and async version and a hidden common block
#define GDAL_ASYNCABLE_DEFINE(method)                                                                                  \
  NAN_METHOD(method) {                                                                                                 \
    method##_do(info, false);                                                                                          \
  }                                                                                                                    \
  NAN_METHOD(method##Async) {                                                                                          \
    method##_do(info, true);                                                                                           \
  }                                                                                                                    \
  void method##_do(const Nan::FunctionCallbackInfo<v8::Value> &info, bool async)

// This generates method declarations for 2 methods: sync and async version and a hidden common block
#define GDAL_ASYNCABLE_DECLARE(method)                                                                                 \
  static NAN_METHOD(method);                                                                                           \
  static NAN_METHOD(method##Async);                                                                                    \
  static void method##_do(const Nan::FunctionCallbackInfo<v8::Value> &info, bool async)

#define GDAL_ASYNCABLE_GLOBAL(method)                                                                                  \
  NAN_METHOD(method);                                                                                                  \
  NAN_METHOD(method##Async);                                                                                           \
  void method##_do(const Nan::FunctionCallbackInfo<v8::Value> &info, bool async)

#define GDAL_ASYNCABLE_TEMPLATE(method)                                                                                \
  static NAN_METHOD(method) {                                                                                          \
    method##_do(info, false);                                                                                          \
  }                                                                                                                    \
  static NAN_METHOD(method##Async) {                                                                                   \
    method##_do(info, true);                                                                                           \
  }                                                                                                                    \
  static void method##_do(const Nan::FunctionCallbackInfo<v8::Value> &info, bool async)

#define GDAL_ISASYNC async

// Handle locking
#define GDAL_TRYLOCK_PARENT(p)                                                                                         \
  std::shared_ptr<uv_sem_t> async_lock = nullptr;                                                                      \
  try {                                                                                                                \
    async_lock = object_store.tryLockDataset((p)->parent_uid);                                                         \
  } catch (const char *err) {                                                                                          \
    Nan::ThrowError(err);                                                                                              \
    return;                                                                                                            \
  }
#define GDAL_ASYNCABLE_LOCK(uid) std::shared_ptr<uv_sem_t> async_lock = object_store.tryLockDataset(uid);
#define GDAL_UNLOCK_PARENT uv_sem_post(async_lock.get())
#define GDAL_ASYNCABLE_LOCK_MANY(...)                                                                                  \
  std::vector<std::shared_ptr<uv_sem_t>> async_locks = object_store.tryLockDatasets({__VA_ARGS__});
#define GDAL_UNLOCK_MANY                                                                                               \
  for (std::shared_ptr<uv_sem_t> & async_lock : async_locks) { uv_sem_post(async_lock.get()); }

// Node.js NAN null initializes and trivially copies objects of this class without asking permission
struct GDALProgressInfo {
  double complete;
  const char *message;

  GDALProgressInfo(const GDALProgressInfo &o);
  GDALProgressInfo(double, const char *);
  GDALProgressInfo();
};

class GDALSyncExecutionProgress {
  Nan::Callback *progress_callback;

  GDALSyncExecutionProgress() = delete;

    public:
  GDALSyncExecutionProgress(Nan::Callback *);
  ~GDALSyncExecutionProgress();
  void Send(GDALProgressInfo *) const;
};

typedef std::function<v8::Local<v8::Value>(const char *)> GetFromPersistentFunc;
typedef Nan::AsyncProgressWorkerBase<GDALProgressInfo> GDALAsyncProgressWorker;
typedef GDALAsyncProgressWorker::ExecutionProgress GDALAsyncExecutionProgress;

// This an ExecutionContext that works both with Node.js' NAN ExecutionProgress when in async mode
// and with GDALSyncExecutionContext when in sync mode
class GDALExecutionProgress {
  // Only one of these is active at any given moment
  const GDALAsyncExecutionProgress *async;
  const GDALSyncExecutionProgress *sync;

  GDALExecutionProgress() = delete;

    public:
  GDALExecutionProgress(const GDALAsyncExecutionProgress *);
  GDALExecutionProgress(const GDALSyncExecutionProgress *);
  ~GDALExecutionProgress();
  void Send(GDALProgressInfo *info) const;
};

/**
 * @typedef ProgressOptions { progress_cb: ProgressCb }
 */

/**
 * @typedef ProgressCb ( complete: number, msg: string ) => void
 */

// This is the progress callback trampoline
// It can be invoked both in the main thread (in sync mode) or in auxillary thread (in async mode)
// It is essentially a gateway between the GDAL world and Node.js/V8 world
int ProgressTrampoline(double dfComplete, const char *pszMessage, void *pProgressArg);

//
// This class handles async operations
// It is meant to be used by GDALAsyncableJob defined below
//
// It takes the lambdas as input
// GDALType is the type of the object that will be carried from
// the aux thread to the main thread
//
// JS-visible object creation is possible only in the main thread while
// ths JS world is not running
//
template <class GDALType> class GDALAsyncWorker : public GDALAsyncProgressWorker {
    public:
  typedef std::function<GDALType(const GDALExecutionProgress &)> GDALMainFunc;
  typedef std::function<v8::Local<v8::Value>(const GDALType, const GetFromPersistentFunc &)> GDALRValFunc;

    private:
  Nan::Callback *progressCallback;
  const GDALMainFunc doit;
  const GDALRValFunc rval;
  GDALType raw;

    public:
  explicit GDALAsyncWorker(
    Nan::Callback *resultCallback,
    Nan::Callback *progressCallback,
    const GDALMainFunc &doit,
    const GDALRValFunc &rval,
    const std::map<std::string, v8::Local<v8::Object>> &objects);

  ~GDALAsyncWorker();

  void Execute(const ExecutionProgress &progress);
  void HandleOKCallback();
  void HandleErrorCallback();
  void HandleProgressCallback(const GDALProgressInfo *data, size_t count);
};

template <class GDALType>
GDALAsyncWorker<GDALType>::GDALAsyncWorker(
  Nan::Callback *resultCallback,
  Nan::Callback *progressCallback,
  const GDALMainFunc &doit,
  const GDALRValFunc &rval,
  const std::map<std::string, v8::Local<v8::Object>> &objects)
  : Nan::AsyncProgressWorkerBase<GDALProgressInfo>(resultCallback, "node-gdal:GDALAsyncWorker"),
    progressCallback(progressCallback),
    // These members are not references! These functions must be copied
    // as they will be executed in async context!
    doit(doit),
    rval(rval) {
  // Main thread with the JS world is not running
  // Get persistent handles
  for (auto i = objects.begin(); i != objects.end(); i++) SaveToPersistent(i->first.c_str(), i->second);
}

template <class GDALType> GDALAsyncWorker<GDALType>::~GDALAsyncWorker() {
  if (progressCallback) delete progressCallback;
}

template <class GDALType> void GDALAsyncWorker<GDALType>::Execute(const ExecutionProgress &progress) {
  // Aux thread with the JS world running
  // V8 objects are not acessible here
  try {
    GDALExecutionProgress executionProgress(&progress);
    raw = doit(executionProgress);
  } catch (const char *err) { this->SetErrorMessage(err); }
}

template <class GDALType> void GDALAsyncWorker<GDALType>::HandleOKCallback() {
  // Back to the main thread with the JS world not running
  Nan::HandleScope scope;

  // rval is the user function that will create the returned value
  // we give it a lambda that can access the persistent storage created for this operation
  v8::Local<v8::Value> argv[] = {
    Nan::Null(), rval(raw, [this](const char *key) { return this->GetFromPersistent(key); })};
  callback->Call(2, argv, async_resource);
}

template <class GDALType> void GDALAsyncWorker<GDALType>::HandleErrorCallback() {
  // Back to the main thread with the JS world not running
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {Nan::Error(this->ErrorMessage())};
  callback->Call(1, argv, async_resource);
}

template <class GDALType>
void GDALAsyncWorker<GDALType>::HandleProgressCallback(const GDALProgressInfo *data, size_t count) {
  // Back to the main thread with the JS world not running
  Nan::HandleScope scope;
  // A mutex-protected pop in the calling function (in Node.js NAN) can sometimes produce a spurious call
  // with no data, handle gracefully this case -> no need to call JS if there is not data to deliver
  if (data == nullptr || count == 0) return;
  // Receiving more than one callback invocation at the same time is also possible
  // Send only the last one to JS
  const GDALProgressInfo *to_send = data + (count - 1);
  if (data != nullptr && count > 0) {
    v8::Local<v8::Value> argv[] = {Nan::New<Number>(to_send->complete), SafeString::New(to_send->message)};
    progressCallback->Call(2, argv, async_resource);
  }
}

// This the basic unit of the GDALAsyncable framework
// GDALAsyncableJob is a GDAL job consisting of a main
// lambda that calls GDAL and rval lambda that transforms
// the return value
// It handles persistence of the referenced objects and
// can be executed both synchronously and asynchronously
//
// GDALType is the intermediary type that will be carried
// across the context boundaries
//
// The caller must ensure that all the lambdas can be executed in
// another thread:
// * no capturing of automatic variables (C++ memory management)
// * no referencing of JS-visible objects in main() (V8 memory management)
// * protecting all JS-visible objects from the GC by calling persist() (V8 MM)
// * locking all GDALDatasets (GDAL limitation)
//
// If a GDALDataset is locked, but not persisted, the GC could still
// try to free it - in this case it will stop the JS world and then it will wait
// on the Dataset lock in PtrManager::dispose() blocking the event loop - the situation
// is safe but it is still best if it is avoided

template <class GDALType> class GDALAsyncableJob {
    public:
  typedef std::function<GDALType(const GDALExecutionProgress &)> GDALMainFunc;
  typedef std::function<v8::Local<v8::Value>(const GDALType, const GetFromPersistentFunc &)> GDALRValFunc;
  // This is the lambda that produces the <GDALType> object
  GDALMainFunc main;
  // This is the lambda that produces the JS return object from the <GDALType> object
  GDALRValFunc rval;
  Nan::Callback *progress;

  GDALAsyncableJob() : main(), rval(), progress(nullptr), persistent(), autoIndex(0){};

  inline void persist(const std::string &key, const v8::Local<v8::Object> &obj) {
    persistent[key] = obj;
  }

  inline void persist(const v8::Local<v8::Object> &obj) {
    persistent[std::to_string(autoIndex++)] = obj;
  }

  inline void persist(const v8::Local<v8::Object> &obj1, const v8::Local<v8::Object> &obj2) {
    persist(obj1);
    persist(obj2);
  }

  inline void persist(const std::vector<v8::Local<v8::Object>> &objs) {
    for (auto const &i : objs) persist(i);
  }

  void run(const Nan::FunctionCallbackInfo<v8::Value> &info, bool async, int cb_arg) {
    if (async) {
      Nan::Callback *callback;
      NODE_ARG_CB(cb_arg, "callback", callback);
      Nan::AsyncQueueWorker(new GDALAsyncWorker<GDALType>(callback, progress, main, rval, persistent));
      return;
    }
    try {
      GDALExecutionProgress executionProgress(new GDALSyncExecutionProgress(progress));
      GDALType obj = main(executionProgress);
      // rval is the user function that will create the returned value
      // we give it a lambda that can access the persistent storage created for this operation
      info.GetReturnValue().Set(rval(obj, [this](const char *key) { return this->persistent[key]; }));
    } catch (const char *err) { Nan::ThrowError(err); }
  }

    private:
  std::map<std::string, v8::Local<v8::Object>> persistent;
  unsigned autoIndex;
};
} // namespace node_gdal
#endif