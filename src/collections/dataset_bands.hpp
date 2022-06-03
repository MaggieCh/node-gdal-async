#ifndef __NODE_GDAL_RASTERBAND_COLLECTION_H__
#define __NODE_GDAL_RASTERBAND_COLLECTION_H__

// node
#include <napi.h>
#include <uv.h>
#include <node_object_wrap.h>

// nan
#include <napi.h>

// gdal
#include <gdal_priv.h>

#include "../async.hpp"

using namespace Napi;
using namespace Napi;

namespace node_gdal {

class DatasetBands : public Napi::ObjectWrap<DatasetBands> {
    public:
  static Napi::FunctionReference constructor;

  static void Initialize(Napi::Object target);
  static Napi::Value New(const Napi::CallbackInfo &info);
  static Napi::Value New(Napi::Value ds_obj);
  static Napi::Value toString(const Napi::CallbackInfo &info);

  GDAL_ASYNCABLE_DECLARE(get);
  GDAL_ASYNCABLE_DECLARE(count);
  GDAL_ASYNCABLE_DECLARE(create);

  Napi::Value dsGetter(const Napi::CallbackInfo &info);

  DatasetBands();

    private:
  ~DatasetBands();
};

} // namespace node_gdal
#endif
