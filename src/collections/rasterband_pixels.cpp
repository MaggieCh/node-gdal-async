#include "rasterband_pixels.hpp"
#include "../gdal_common.hpp"
#include "../gdal_rasterband.hpp"
#include "../async.hpp"
#include "../utils/typed_array.hpp"

#include <sstream>

namespace node_gdal {

Napi::FunctionReference RasterBandPixels::constructor;

void RasterBandPixels::Initialize(Napi::Object target) {
  Napi::HandleScope scope(env);

  Napi::FunctionReference lcons = Napi::Function::New(env, RasterBandPixels::New);

  lcons->SetClassName(Napi::String::New(env, "RasterBandPixels"));

  InstanceMethod("toString", &toString), Nan__SetPrototypeAsyncableMethod(lcons, "get", get);
  Nan__SetPrototypeAsyncableMethod(lcons, "set", set);
  Nan__SetPrototypeAsyncableMethod(lcons, "read", read);
  Nan__SetPrototypeAsyncableMethod(lcons, "write", write);
  Nan__SetPrototypeAsyncableMethod(lcons, "readBlock", readBlock);
  Nan__SetPrototypeAsyncableMethod(lcons, "writeBlock", writeBlock);
  Nan__SetPrototypeAsyncableMethod(lcons, "clampBlock", clampBlock);

  ATTR_DONT_ENUM(lcons, "band", bandGetter, READ_ONLY_SETTER);

  (target).Set(Napi::String::New(env, "RasterBandPixels"), Napi::GetFunction(lcons));

  constructor.Reset(lcons);
}

RasterBandPixels::RasterBandPixels() : Napi::ObjectWrap<RasterBandPixels>() {
}

RasterBandPixels::~RasterBandPixels() {
}

RasterBand *RasterBandPixels::parent(const Napi::CallbackInfo &info) {
  Napi::Object parent = Napi::GetPrivate(info.This(), Napi::String::New(env, "parent_")).As<Napi::Object>();
  RasterBand *band = parent.Unwrap<RasterBand>();
  if (!band->isAlive()) {
    Napi::Error::New(env, "RasterBand object has already been destroyed").ThrowAsJavaScriptException();

    return nullptr;
  }
  return band;
}

/**
 * A representation of a {@link RasterBand}'s pixels.
 *
 *
 * Note: Typed arrays should be created with an external ArrayBuffer for
 * versions of node >= 0.11
 *
 * @example
 * const n = 16*16;
 * const data = new Float32Array(new ArrayBuffer(n*4));
 * //read data into the existing array
 * band.pixels.read(0,0,16,16,data);
 *
 * @class RasterBandPixels
 */
Napi::Value RasterBandPixels::New(const Napi::CallbackInfo &info) {

  if (!info.IsConstructCall()) {
    Napi::Error::New(env, "Cannot call constructor as function, you need to use 'new' keyword")
      .ThrowAsJavaScriptException();
    return env.Null();
  }
  if (info[0].IsExternal()) {
    Napi::External ext = info[0].As<Napi::External>();
    void *ptr = ext->Value();
    RasterBandPixels *f = static_cast<RasterBandPixels *>(ptr);
    f->Wrap(info.This());
    return info.This();
    return;
  } else {
    Napi::Error::New(env, "Cannot create RasterBandPixels directly").ThrowAsJavaScriptException();
    return env.Null();
  }
}

Napi::Value RasterBandPixels::New(Napi::Value band_obj) {
  Napi::Env env = band_obj.Env();
  Napi::EscapableHandleScope scope(env);

  RasterBandPixels *wrapped = new RasterBandPixels();

  Napi::Value ext = Napi::External::New(env, wrapped);
  Napi::Object obj = Napi::NewInstance(Napi::GetFunction(Napi::New(env, RasterBandPixels::constructor)), 1, &ext);
  Napi::SetPrivate(obj, Napi::String::New(env, "parent_"), band_obj);

  return scope.Escape(obj);
}

Napi::Value RasterBandPixels::toString(const Napi::CallbackInfo &info) {
  return Napi::String::New(env, "RasterBandPixels");
}

/**
 * Returns the value at the x, y coordinate.
 *
 * @method get
 * @instance
 * @memberof RasterBandPixels
 * @param {number} x
 * @param {number} y
 * @throws {Error}
 * @return {number}
 */

/**
 * Returns the value at the x, y coordinate.
 * @async
 *
 * @method getAsync
 * @instance
 * @memberof RasterBandPixels
 * @param {number} x
 * @param {number} y
 * @param {callback<number>} [callback=undefined]
 * @return {Promise<number>}
 */
GDAL_ASYNCABLE_DEFINE(RasterBandPixels::get) {

  RasterBand *band;
  if ((band = parent(info)) == nullptr) return;

  int x, y;

  NODE_ARG_INT(0, "x", x);
  NODE_ARG_INT(1, "y", y);
  GDALRasterBand *raw = band->get();

  GDALAsyncableJob<double> job(band->parent_uid);
  job.persist(band->handle());

  job.main = [raw, x, y](const GDALExecutionProgress &) {
    double val;
    CPLErrorReset();
    CPLErr err = raw->RasterIO(GF_Read, x, y, 1, 1, &val, 1, 1, GDT_Float64, 0, 0);
    if (err) { throw CPLGetLastErrorMsg(); }
    return val;
  };

  job.rval = [](double val, const GetFromPersistentFunc &) { return Napi::Number::New(env, val); };
  job.run(info, async, 2);
}

/**
 * Sets the value at the x, y coordinate.
 *
 * @method set
 * @instance
 * @memberof RasterBandPixels
 * @param {number} x
 * @param {number} y
 * @param {number} value
 */

/**
 * Sets the value at the x, y coordinate.
 * @async
 *
 * @method setAsync
 * @instance
 * @memberof RasterBandPixels
 * @param {number} x
 * @param {number} y
 * @param {number} value
 * @param {callback<void>} [callback=undefined]
 * @return {Promise<void>}
 */
GDAL_ASYNCABLE_DEFINE(RasterBandPixels::set) {

  RasterBand *band;
  if ((band = parent(info)) == nullptr) return;

  int x, y;
  double val;

  NODE_ARG_INT(0, "x", x);
  NODE_ARG_INT(1, "y", y);
  NODE_ARG_DOUBLE(2, "val", val);
  GDALRasterBand *raw = band->get();

  GDALAsyncableJob<CPLErr> job(band->parent_uid);
  job.persist(band->handle());

  job.main = [raw, x, y, val](const GDALExecutionProgress &) {
    CPLErrorReset();
    CPLErr err = raw->RasterIO(GF_Write, x, y, 1, 1, (void *)&val, 1, 1, GDT_Float64, 0, 0);
    if (err) { throw CPLGetLastErrorMsg(); }
    return err;
  };

  job.rval = [](CPLErr r, const GetFromPersistentFunc &) { return env.Undefined(); };
  job.run(info, async, 3);
}

inline GDALRIOResampleAlg parseResamplingAlg(Napi::Value value) {
  Napi::Env env = value.Env();
  if (value.IsNull() || value.IsNull()) { return GRIORA_NearestNeighbour; }
  if (!value.IsString()) { throw "resampling property must be a string"; }
  std::string name = value.As<Napi::String>().Utf8Value().c_str();

  if (name == "NearestNeighbor") { return GRIORA_NearestNeighbour; }
  if (name == "NearestNeighbour") { return GRIORA_NearestNeighbour; }
  if (name == "Bilinear") { return GRIORA_Bilinear; }
  if (name == "Cubic") { return GRIORA_Cubic; }
  if (name == "CubicSpline") { return GRIORA_CubicSpline; }
  if (name == "Lanczos") { return GRIORA_Lanczos; }
  if (name == "Average") { return GRIORA_Average; }
  if (name == "Mode") { return GRIORA_Mode; }
  if (name == "Gauss") { return GRIORA_Gauss; }

  throw "Invalid resampling algorithm";
}

/* Find the lowest possible element index for the given width, height, pixel_space, line_space and offset */
static inline int findLowest(int w, int h, int px, int ln, int offset) {
  int x, y;

  if (px < 0)
    x = w - 1;
  else
    x = 0;

  if (ln < 0)
    y = h - 1;
  else
    y = 0;

  return offset + (x * px + y * ln);
}

/* Find the highest possible element index for the given width, height, pixel_space, line_space and offset */
static inline int findHighest(int w, int h, int px, int ln, int offset) {
  int x, y;

  if (px < 0)
    x = 0;
  else
    x = w - 1;

  if (ln < 0)
    y = 0;
  else
    y = h - 1;

  return offset + (x * px + y * ln);
}

/**
 * @typedef {Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array} TypedArray
 * @memberof RasterBandPixels
 */

/**
 * @typedef {object} ReadOptions
 * @memberof RasterBandPixels
 * @property {number} [buffer_width]
 * @property {number} [buffer_height]
 * @property {string} [type]
 * @property {number} [pixel_space]
 * @property {number} [line_space]
 * @property {string} [resampling]
 * @property {ProgressCb} [progress_cb]
 * @property {number} [offset]
 */

/**
 * Reads a region of pixels.
 *
 * @method read
 * @instance
 * @memberof RasterBandPixels
 * @throws {Error}
 * @param {number} x
 * @param {number} y
 * @param {number} width
 * @param {number} height
 * @param {TypedArray} [data] The `TypedArray` to put the data in. A new array is created if not given.
 * @param {ReadOptions} [options]
 * @param {number} [options.buffer_width=x_size]
 * @param {number} [options.buffer_height=y_size]
 * @param {string} [options.data_type] See {@link GDT|GDT constants}
 * @param {number} [options.pixel_space]
 * @param {number} [options.line_space]
 * @param {string} [options.resampling] Resampling algorithm ({@link GRA|available options})
 * @param {ProgressCb} [options.progress_cb]
 * @return {TypedArray} A `TypedArray` of values.
 */

/**
 * Asynchronously reads a region of pixels.
 * @async
 *
 * @method readAsync
 * @instance
 * @memberof RasterBandPixels
 * @param {number} x
 * @param {number} y
 * @param {number} width the width
 * @param {number} height
 * @param {TypedArray} [data] The `TypedArray` to put the data in. A new array is created if not given.
 * @param {ReadOptions} [options]
 * @param {number} [options.buffer_width=x_size]
 * @param {number} [options.buffer_height=y_size]
 * @param {string} [options.data_type] See {@link GDT|GDT constants}
 * @param {number} [options.pixel_space]
 * @param {number} [options.line_space]
 * @param {string} [options.resampling] Resampling algorithm ({@link GRA|available options}
 * @param {ProgressCb} [options.progress_cb]
 * @param {callback<TypedArray>} [callback=undefined]
 * @return {Promise<TypedArray>} A `TypedArray` of values.
 */
GDAL_ASYNCABLE_DEFINE(RasterBandPixels::read) {

  RasterBand *band;
  if ((band = parent(info)) == nullptr) return;

  int x, y, w, h;
  int buffer_w, buffer_h;
  int bytes_per_pixel;
  int pixel_space, line_space;
  int size, length, offset;
  void *data;
  Napi::Value array;
  Napi::Object obj;
  Napi::FunctionReference *cb = nullptr;
  GDALDataType type;

  NODE_ARG_INT(0, "x_offset", x);
  NODE_ARG_INT(1, "y_offset", y);
  NODE_ARG_INT(2, "x_size", w);
  NODE_ARG_INT(3, "y_size", h);

  std::string type_name = "";

  buffer_w = w;
  buffer_h = h;
  type = band->get()->GetRasterDataType();
  NODE_ARG_INT_OPT(5, "buffer_width", buffer_w);
  NODE_ARG_INT_OPT(6, "buffer_height", buffer_h);
  NODE_ARG_OPT_STR(7, "data_type", type_name);
  if (!type_name.empty()) { type = GDALGetDataTypeByName(type_name.c_str()); }

  if (!info[4].IsUndefined() && !info[4].IsNull()) {
    NODE_ARG_OBJECT(4, "data", obj);
    type = TypedArray::Identify(obj);
    if (type == GDT_Unknown) {
      Napi::Error::New(env, "Invalid array").ThrowAsJavaScriptException();
      return env.Null();
    }
  }

  bytes_per_pixel = GDALGetDataTypeSize(type) / 8;
  pixel_space = bytes_per_pixel;
  NODE_ARG_INT_OPT(8, "pixel_space", pixel_space);
  line_space = pixel_space * buffer_w;
  NODE_ARG_INT_OPT(9, "line_space", line_space);
  NODE_ARG_CB_OPT(11, "progress_cb", cb);
  GDALRIOResampleAlg resampling;
  try {
    resampling = parseResamplingAlg(info[10]);
  } catch (const char *e) {
    Napi::Error::New(env, e).ThrowAsJavaScriptException();
    return env.Null();
  }
  offset = 0;
  NODE_ARG_INT_OPT(12, "offset", offset);

  if (findLowest(buffer_w, buffer_h, pixel_space, line_space, offset) < 0) {
    Napi::Error::New(env, "has to write before the start of the TypedArray").ThrowAsJavaScriptException();
    return env.Null();
  }
  size = findHighest(buffer_w, buffer_h, pixel_space, line_space, offset) + 1;
  // length (elements) = size / bytes_per_pixel + 1 more if it is not a perfect fit
  length = size / bytes_per_pixel + ((size % bytes_per_pixel) ? 1 : 0);

  // create array if no array was passed
  if (obj.IsEmpty()) {
    array = TypedArray::New(type, length);
    if (array.IsEmpty() || !array.IsObject()) {
      return; // TypedArray::New threw an error
    }
    obj = array.As<Napi::Object>();
  }

  data = TypedArray::Validate(obj, type, length);
  if (!data) {
    return; // TypedArray::Validate threw an error
  }

  GDALRasterBand *gdal_band = band->get();
  GDALAsyncableJob<CPLErr> job(band->parent_uid);
  job.persist("array", obj);
  job.persist(band->handle());
  job.progress = cb;

  data = (uint8_t *)data + offset * bytes_per_pixel;
  job.main = [gdal_band, x, y, w, h, data, buffer_w, buffer_h, type, pixel_space, line_space, resampling, cb](
               const GDALExecutionProgress &progress) {
    std::shared_ptr<GDALRasterIOExtraArg> extra(new GDALRasterIOExtraArg);
    INIT_RASTERIO_EXTRA_ARG(*extra);
    extra->eResampleAlg = resampling;
    if (cb) {
      extra->pfnProgress = ProgressTrampoline;
      extra->pProgressData = (void *)&progress;
    }

    CPLErrorReset();
    CPLErr err =
      gdal_band->RasterIO(GF_Read, x, y, w, h, data, buffer_w, buffer_h, type, pixel_space, line_space, extra.get());

    if (err != CE_None) throw CPLGetLastErrorMsg();
    return err;
  };

  job.rval = [](CPLErr err, const GetFromPersistentFunc &getter) { return getter("array"); };
  job.run(info, async, 13);
}

/**
 * @typedef {object} WriteOptions
 * @memberof RasterBandPixels
 * @property {number} [buffer_width]
 * @property {number} [buffer_height]
 * @property {number} [pixel_space]
 * @property {number} [line_space]
 * @property {ProgressCb} [progress_cb]
 * @property {number} [offset]
 */

/**
 * Writes a region of pixels.
 *
 * @method write
 * @instance
 * @memberof RasterBandPixels
 * @throws {Error}
 * @param {number} x
 * @param {number} y
 * @param {number} width
 * @param {number} height
 * @param {TypedArray} [data] The `TypedArray` to write to the band.
 * @param {WriteOptions} [options]
 * @param {number} [options.buffer_width=x_size]
 * @param {number} [options.buffer_height=y_size]
 * @param {number} [options.pixel_space]
 * @param {number} [options.line_space]
 * @param {ProgressCb} [options.progress_cb]
 */

/**
 * Asynchronously writes a region of pixels.
 * @async
 *
 * @method writeAsync
 * @instance
 * @memberof RasterBandPixels
 * @param {number} x
 * @param {number} y
 * @param {number} width
 * @param {number} height
 * @param {TypedArray} [data] The `TypedArray` to write to the band.
 * @param {WriteOptions} [options]
 * @param {number} [options.buffer_width=x_size]
 * @param {number} [options.buffer_height=y_size]
 * @param {number} [options.pixel_space]
 * @param {number} [options.line_space]
 * @param {ProgressCb} [options.progress_cb]
 * @param {callback<void>} [callback=undefined]
 * @return {Promise<void>}
 */
GDAL_ASYNCABLE_DEFINE(RasterBandPixels::write) {

  RasterBand *band;
  if ((band = parent(info)) == nullptr) return;

  int x, y, w, h;
  int buffer_w, buffer_h;
  int bytes_per_pixel;
  int pixel_space, line_space;
  int size, length, offset;
  void *data;
  Napi::Object passed_array;
  GDALDataType type;
  Napi::FunctionReference *cb = nullptr;

  NODE_ARG_INT(0, "x_offset", x);
  NODE_ARG_INT(1, "y_offset", y);
  NODE_ARG_INT(2, "x_size", w);
  NODE_ARG_INT(3, "y_size", h);
  NODE_ARG_OBJECT(4, "data", passed_array);

  buffer_w = w;
  buffer_h = h;
  NODE_ARG_INT_OPT(5, "buffer_width", buffer_w);
  NODE_ARG_INT_OPT(6, "buffer_height", buffer_h);

  type = TypedArray::Identify(passed_array);
  if (type == GDT_Unknown) {
    Napi::Error::New(env, "Invalid array").ThrowAsJavaScriptException();
    return env.Null();
  }

  bytes_per_pixel = GDALGetDataTypeSize(type) / 8;
  pixel_space = bytes_per_pixel;
  NODE_ARG_INT_OPT(7, "pixel_space", pixel_space);
  line_space = pixel_space * buffer_w;
  NODE_ARG_INT_OPT(8, "line_space", line_space);
  NODE_ARG_CB_OPT(9, "progress_cb", cb);
  offset = 0;
  NODE_ARG_INT_OPT(10, "offset", offset);

  if (findLowest(buffer_w, buffer_h, pixel_space, line_space, offset) < 0) {
    Napi::Error::New(env, "has to read before the start of the TypedArray").ThrowAsJavaScriptException();
    return env.Null();
  }
  size = findHighest(buffer_w, buffer_h, pixel_space, line_space, offset) + 1;
  // length (elements) = size / bytes_per_pixel + 1 more if it is not a perfect fit
  length = size / bytes_per_pixel + ((size % bytes_per_pixel) ? 1 : 0);

  data = TypedArray::Validate(passed_array, type, length);
  if (!data) {
    return; // TypedArray::Validate threw an error
  }

  GDALRasterBand *gdal_band = band->get();
  GDALAsyncableJob<CPLErr> job(band->parent_uid);
  job.persist("array", passed_array);
  job.persist(band->handle());
  if (cb) {
    job.persist(cb->GetFunction());
    job.progress = cb;
  }

  data = (uint8_t *)data + offset * bytes_per_pixel;
  job.main = [gdal_band, x, y, w, h, data, buffer_w, buffer_h, type, pixel_space, line_space, cb](
               const GDALExecutionProgress &progress) {
    std::shared_ptr<GDALRasterIOExtraArg> extra(new GDALRasterIOExtraArg);
    INIT_RASTERIO_EXTRA_ARG(*extra);
    if (cb) {
      extra->pfnProgress = ProgressTrampoline;
      extra->pProgressData = (void *)&progress;
    }

    CPLErrorReset();
    CPLErr err =
      gdal_band->RasterIO(GF_Write, x, y, w, h, data, buffer_w, buffer_h, type, pixel_space, line_space, extra.get());
    if (err != CE_None) throw CPLGetLastErrorMsg();
    return err;
  };
  job.rval = [](CPLErr, const GetFromPersistentFunc &getter) { return getter("array"); };

  job.run(info, async, 11);
}

/**
 * Reads a block of pixels.
 *
 * @method readBlock
 * @instance
 * @memberof RasterBandPixels
 * @throws {Error}
 * @param {number} x
 * @param {number} y
 * @param {TypedArray} [data] The `TypedArray` to put the data in. A new array is created if not given.
 * @return {TypedArray} A `TypedArray` of values.
 */

/**
 * Reads a block of pixels.
 * @async
 *
 * @method readBlockAsync
 * @instance
 * @memberof RasterBandPixels
 * @throws {Error}
 * @param {number} x
 * @param {number} y
 * @param {TypedArray} [data] The `TypedArray` to put the data in. A new array is created if not given.
 * @param {callback<TypedArray>} [callback=undefined]
 * @return {Promise<TypedArray>} A `TypedArray` of values.
 */

GDAL_ASYNCABLE_DEFINE(RasterBandPixels::readBlock) {

  RasterBand *band;
  if ((band = parent(info)) == nullptr) return;

  int x, y, w = 0, h = 0;
  NODE_ARG_INT(0, "block_x_offset", x);
  NODE_ARG_INT(1, "block_y_offset", y);

  band->get()->GetBlockSize(&w, &h);

  GDALDataType type = band->get()->GetRasterDataType();

  Napi::Value array;
  Napi::Object obj;

  if (info.Length() > 2 && !info[2].IsUndefined() && !info[2].IsNull()) {
    NODE_ARG_OBJECT(2, "data", obj);
    array = obj;
  } else {
    array = TypedArray::New(type, w * h);
    if (array.IsEmpty() || !array.IsObject()) {
      return; // TypedArray::New threw an error
    }
    obj = array.As<Napi::Object>();
  }

  void *data = TypedArray::Validate(obj, type, w * h);
  if (!data) {
    return; // TypedArray::Validate threw an error
  }

  GDALRasterBand *gdal_band = band->get();

  GDALAsyncableJob<CPLErr> job(band->parent_uid);
  job.persist("array", obj);
  job.persist(band->handle());
  job.main = [gdal_band, x, y, data](const GDALExecutionProgress &) {
    CPLErrorReset();
    CPLErr err = gdal_band->ReadBlock(x, y, data);
    if (err) { throw CPLGetLastErrorMsg(); }
    return err;
  };
  job.rval = [](CPLErr r, const GetFromPersistentFunc &getter) { return getter("array"); };
  job.run(info, async, 3);
}

/**
 * Writes a block of pixels.
 *
 * @method writeBlock
 * @instance
 * @memberof RasterBandPixels
 * @throws {Error}
 * @param {number} x
 * @param {number} y
 * @param {TypedArray} data The `TypedArray` of values to write to the band.
 */

/**
 * Writes a block of pixels.
 * @async
 *
 * @method writeBlockAsync
 * @instance
 * @memberof RasterBandPixels
 * @throws {Error}
 * @param {number} x
 * @param {number} y
 * @param {TypedArray} data The `TypedArray` of values to write to the band.
 * @param {callback<void>} [callback=undefined]
 * @return {Promise<void>}
 */
GDAL_ASYNCABLE_DEFINE(RasterBandPixels::writeBlock) {

  RasterBand *band;
  if ((band = parent(info)) == nullptr) return;

  int x, y, w = 0, h = 0;

  band->get()->GetBlockSize(&w, &h);

  NODE_ARG_INT(0, "block_x_offset", x);
  NODE_ARG_INT(1, "block_y_offset", y);

  Napi::Object obj;
  NODE_ARG_OBJECT(2, "data", obj);

  // validate array
  void *data = TypedArray::Validate(obj, band->get()->GetRasterDataType(), w * h);
  if (!data) {
    return; // TypedArray::Validate threw an error
  }

  GDALRasterBand *gdal_band = band->get();

  GDALAsyncableJob<CPLErr> job(band->parent_uid);
  job.persist(obj, band->handle());
  job.main = [gdal_band, x, y, data](const GDALExecutionProgress &) {
    CPLErrorReset();
    CPLErr err = gdal_band->WriteBlock(x, y, data);
    if (err) { throw CPLGetLastErrorMsg(); }
    return err;
  };
  job.rval = [](CPLErr r, const GetFromPersistentFunc &) { return env.Undefined(); };
  job.run(info, async, 3);
}

/**
 * Clamp the block size for a given block offset.
 * Handles partial blocks at the edges of the raster and returns the true number of pixels.
 *
 * @method clampBlock
 * @instance
 * @memberof RasterBandPixels
 * @throws {Error}
 * @param {number} x
 * @param {number} y
 * @return {xyz} A size object.
 */

/**
 * Clamp the block size for a given block offset.
 * Handles partial blocks at the edges of the raster and returns the true number of pixels.
 * @async
 *
 * @method clampBlockAsync
 * @instance
 * @memberof RasterBandPixels
 * @throws {Error}
 * @param {number} x
 * @param {number} y
 * @param {callback<xyz>} [callback=undefined]
 * @return {Promise<xyz>} A size object.
 */

GDAL_ASYNCABLE_DEFINE(RasterBandPixels::clampBlock) {

  RasterBand *band;
  if ((band = parent(info)) == nullptr) return;

  int x, y;
  NODE_ARG_INT(0, "block_x_offset", x);
  NODE_ARG_INT(1, "block_y_offset", y);

  struct xy {
    int x, y;
  };
  GDALRasterBand *gdal_band = band->get();
  GDALAsyncableJob<xy> job(band->parent_uid);
  job.persist(band->handle());
  job.main = [gdal_band, x, y](const GDALExecutionProgress &) {
    xy r;
    CPLErrorReset();
    CPLErr err = gdal_band->GetActualBlockSize(x, y, &r.x, &r.y);
    if (err != CE_None) { throw CPLGetLastErrorMsg(); }
    return r;
  };
  job.rval = [](xy r, const GetFromPersistentFunc &) {
    Napi::EscapableHandleScope scope(env);
    Napi::Object result = Napi::Object::New(env);
    (result).Set(Napi::String::New(env, "x"), Napi::Number::New(env, r.x));
    (result).Set(Napi::String::New(env, "y"), Napi::Number::New(env, r.y));
    return scope.Escape(result.As<Napi::Value>());
  };
  job.run(info, async, 2);
}

/**
 * Returns the parent raster band.
 *
 * @readonly
 * @kind member
 * @name band
 * @instance
 * @memberof RasterBandPixels
 * @type {RasterBand}
 */
Napi::Value RasterBandPixels::bandGetter(const Napi::CallbackInfo &info) {
  return Napi::GetPrivate(info.This(), Napi::String::New(env, "parent_"));
}

} // namespace node_gdal
