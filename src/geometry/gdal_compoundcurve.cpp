
#include "gdal_compoundcurve.hpp"
#include "../collections/linestring_points.hpp"
#include "../gdal_common.hpp"
#include "gdal_geometry.hpp"
#include "gdal_point.hpp"

#include <stdlib.h>

namespace node_gdal {

#if GDAL_VERSION_MAJOR >= 2

Nan::Persistent<FunctionTemplate> CompoundCurve::constructor;

void CompoundCurve::Initialize(Local<Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> lcons = Nan::New<FunctionTemplate>(CompoundCurve::New);
  lcons->Inherit(Nan::New(Geometry::constructor));
  lcons->InstanceTemplate()->SetInternalFieldCount(1);
  lcons->SetClassName(Nan::New("CompoundCurve").ToLocalChecked());

  Nan::SetPrototypeMethod(lcons, "toString", toString);

  ATTR(lcons, "curves", curvesGetter, READ_ONLY_SETTER);

  Nan::Set(target, Nan::New("CompoundCurve").ToLocalChecked(), Nan::GetFunction(lcons).ToLocalChecked());

  constructor.Reset(lcons);
}

void CompoundCurve::SetPrivate(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE _this, v8::Local<v8::Value> value) {
  Nan::SetPrivate(_this, Nan::New("curves_").ToLocalChecked(), value);
};

/**
 * Concrete representation of a compound contionuos curve.
 *
 * @example
 * ```
 * var CompoundCurve = new gdal.CompoundCurve();
 * CompoundCurve.points.add(new gdal.Point(0,0));
 * CompoundCurve.points.add(new gdal.Point(0,10));```
 *
 * @constructor
 * @class gdal.CompoundCurve
 * @extends gdal.Geometry
 */

NAN_METHOD(CompoundCurve::toString) {
  Nan::HandleScope scope;
  info.GetReturnValue().Set(Nan::New("CompoundCurve").ToLocalChecked());
}

/**
 * Points that make up the compound curve.
 *
 * @attribute points
 * @type {gdal.CompoundCurvePoints}
 */
NAN_GETTER(CompoundCurve::curvesGetter) {
  Nan::HandleScope scope;
  info.GetReturnValue().Set(Nan::GetPrivate(info.This(), Nan::New("curves_").ToLocalChecked()).ToLocalChecked());
}

#endif

} // namespace node_gdal
