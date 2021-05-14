#include <memory>
#include "dataset_bands.hpp"
#include "../gdal_common.hpp"
#include "../gdal_dataset.hpp"
#include "../gdal_rasterband.hpp"
#include "../utils/string_list.hpp"

namespace node_gdal {

Nan::Persistent<FunctionTemplate> DatasetBands::constructor;

void DatasetBands::Initialize(Local<Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> lcons = Nan::New<FunctionTemplate>(DatasetBands::New);
  lcons->InstanceTemplate()->SetInternalFieldCount(1);
  lcons->SetClassName(Nan::New("DatasetBands").ToLocalChecked());

  Nan::SetPrototypeMethod(lcons, "toString", toString);
  Nan__SetPrototypeAsyncableMethod(lcons, "count", count);
  Nan__SetPrototypeAsyncableMethod(lcons, "create", create);
  Nan__SetPrototypeAsyncableMethod(lcons, "get", get);

  ATTR_DONT_ENUM(lcons, "ds", dsGetter, READ_ONLY_SETTER);

  Nan::Set(target, Nan::New("DatasetBands").ToLocalChecked(), Nan::GetFunction(lcons).ToLocalChecked());

  constructor.Reset(lcons);
}

DatasetBands::DatasetBands() : Nan::ObjectWrap() {
}

DatasetBands::~DatasetBands() {
}

/**
 * An encapsulation of a {{#crossLink "gdal.Dataset"}}Dataset{{/crossLink}}'s
 * raster bands.
 *
 * ```
 * var bands = dataset.bands;```
 *
 * @class gdal.DatasetBands
 */
NAN_METHOD(DatasetBands::New) {
  Nan::HandleScope scope;

  if (!info.IsConstructCall()) {
    Nan::ThrowError("Cannot call constructor as function, you need to use 'new' keyword");
    return;
  }
  if (info[0]->IsExternal()) {
    Local<External> ext = info[0].As<External>();
    void *ptr = ext->Value();
    DatasetBands *f = static_cast<DatasetBands *>(ptr);
    f->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
    return;
  } else {
    Nan::ThrowError("Cannot create DatasetBands directly");
    return;
  }
}

Local<Value> DatasetBands::New(Local<Value> ds_obj) {
  Nan::EscapableHandleScope scope;

  DatasetBands *wrapped = new DatasetBands();

  v8::Local<v8::Value> ext = Nan::New<External>(wrapped);
  v8::Local<v8::Object> obj =
    Nan::NewInstance(Nan::GetFunction(Nan::New(DatasetBands::constructor)).ToLocalChecked(), 1, &ext).ToLocalChecked();
  Nan::SetPrivate(obj, Nan::New("parent_").ToLocalChecked(), ds_obj);

  return scope.Escape(obj);
}

NAN_METHOD(DatasetBands::toString) {
  Nan::HandleScope scope;
  info.GetReturnValue().Set(Nan::New("DatasetBands").ToLocalChecked());
}

/**
 * Returns the band with the given ID.
 *
 * @method get
 * @param {number} id
 * @return {gdal.RasterBand}
 */

/**
 * Returns the band with the given ID.
 * {{{async}}}
 *
 * @method getAsync
 *
 * @param {number} id
 * @param {callback<gdal.RasterBand>} [callback=undefined] {{{cb}}}
 * @return {Promise<gdal.RasterBand>}
 */
GDAL_ASYNCABLE_DEFINE(DatasetBands::get) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  Dataset *ds = Nan::ObjectWrap::Unwrap<Dataset>(parent);

  if (!ds->isAlive()) {
    Nan::ThrowError("Dataset object has already been destroyed");
    return;
  }

#if GDAL_VERSION_MAJOR < 2
  GDAL_ASYNCABLE_1x_UNSUPPORTED;
  if (ds->uses_ogr) {
    info.GetReturnValue().Set(Nan::Null());
    return;
  } else {
#else
  {
#endif
    GDALDataset *raw = ds->getDataset();
    long ds_uid = ds->uid;
    int band_id;
    NODE_ARG_INT(0, "band id", band_id);

    GDALAsyncableJob<GDALRasterBand *> job;
    job.persist(parent);
    job.main = [ds_uid, raw, band_id]() {
      GDAL_ASYNCABLE_LOCK(ds_uid);
      GDALRasterBand *band = raw->GetRasterBand(band_id);
      GDAL_UNLOCK_PARENT;
      return band;
    };
    job.rval = [raw](GDALRasterBand *band, GDAL_ASYNCABLE_OBJS) { return RasterBand::New(band, raw); };
    job.run(info, async, 1);
  }
}

/**
 * Adds a new band.
 *
 * @method create
 * @throws Error
 * @param {string} dataType Type of band ({{#crossLink "Constants (GDT)"}}see GDT constants{{/crossLink}}).
 * @param {object} [options] Creation options
 * @return {gdal.RasterBand}
 */

/**
 * Adds a new band.
 * {{{async}}}
 *
 * @method createAsync
 * @throws Error
 * @param {string} dataType Type of band ({{#crossLink "Constants (GDT)"}}see GDT constants{{/crossLink}}).
 * @param {object} [options] Creation options
 * @param {callback<gdal.RasterBand>} [callback=undefined] {{{cb}}}
 * @return {Promise<gdal.RasterBand>}
 */

GDAL_ASYNCABLE_DEFINE(DatasetBands::create) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  Dataset *ds = Nan::ObjectWrap::Unwrap<Dataset>(parent);

  if (!ds->isAlive()) {
    Nan::ThrowError("Dataset object has already been destroyed");
    return;
  }

#if GDAL_VERSION_MAJOR < 2
  GDAL_ASYNCABLE_1x_UNSUPPORTED;
  if (ds->uses_ogr) {
    Nan::ThrowError("Dataset does not support getting creating bands");
    return;
  }
#endif

  GDALDataset *raw = ds->getDataset();
  GDALDataType type;
  StringList *options = new StringList;

  // NODE_ARG_ENUM(0, "data type", GDALDataType, type);
  if (info.Length() < 1) {
    Nan::ThrowError("data type argument needed");
    return;
  }
  if (info[0]->IsString()) {
    std::string type_name = *Nan::Utf8String(info[0]);
    type = GDALGetDataTypeByName(type_name.c_str());
  } else if (info[0]->IsNull() || info[0]->IsUndefined()) {
    type = GDT_Unknown;
  } else {
    Nan::ThrowError("data type must be string or undefined");
    return;
  }

  if (info.Length() > 1 && options->parse(info[1])) {
    return; // error parsing creation options, options->parse does the throwing
  }

  long ds_uid = ds->uid;

  GDALAsyncableJob<GDALRasterBand *> job;
  job.persist(parent);
  job.main = [ds_uid, raw, type, options]() {
    std::unique_ptr<StringList> options_ptr(options);
    GDAL_ASYNCABLE_LOCK(ds_uid);
    CPLErr err = raw->AddBand(type, options->get());
    GDAL_UNLOCK_PARENT;
    // This is not thread-safe, but mis-reporting the error message
    // 0.01% of the time is probably acceptable
    if (err != CE_None) { throw CPLGetLastErrorMsg(); }
    return raw->GetRasterBand(raw->GetRasterCount());
  };
  job.rval = [raw](GDALRasterBand *r, GDAL_ASYNCABLE_OBJS) { return RasterBand::New(r, raw); };
  job.run(info, async, 2);
}

/**
 * Returns the number of bands.
 *
 * @method count
 * @return {number}
 */

/**
 * 
 * Returns the number of bands.
 * {{{async}}}
 *
 * @method countAsync
 *
 * @param {callback<number>} [callback=undefined] {{{cb}}}
 * @return {Promise<number>}
 */
GDAL_ASYNCABLE_DEFINE(DatasetBands::count) {
  Nan::HandleScope scope;

  Local<Object> parent =
    Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked().As<Object>();
  Dataset *ds = Nan::ObjectWrap::Unwrap<Dataset>(parent);

  if (!ds->isAlive()) {
    Nan::ThrowError("Dataset object has already been destroyed");
    return;
  }

#if GDAL_VERSION_MAJOR < 2
  GDAL_ASYNCABLE_1x_UNSUPPORTED;
  if (ds->uses_ogr) {
    info.GetReturnValue().Set(Nan::New<Integer>(0));
    return;
  }
#endif

  long ds_uid = ds->uid;
  GDALDataset *raw = ds->getDataset();
  GDALAsyncableJob<int> job;
  job.persist(parent);
  job.main = [ds_uid, raw]() {
    GDAL_ASYNCABLE_LOCK(ds_uid);
    int count = raw->GetRasterCount();
    GDAL_UNLOCK_PARENT;
    return count;
  };
  job.rval = [](int count, GDAL_ASYNCABLE_OBJS) { return Nan::New<Integer>(count); };
  job.run(info, async, 0);
}

/**
 * Parent dataset
 *
 * @readOnly
 * @attribute ds
 * @type {gdal.Dataset}
 */
NAN_GETTER(DatasetBands::dsGetter) {
  Nan::HandleScope scope;
  info.GetReturnValue().Set(Nan::GetPrivate(info.This(), Nan::New("parent_").ToLocalChecked()).ToLocalChecked());
}

} // namespace node_gdal
