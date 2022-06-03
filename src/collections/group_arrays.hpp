#ifndef __NODE_GDAL_ARRAYS_COLLECTION_H__
#define __NODE_GDAL_ARRAYS_COLLECTION_H__

// node
#include <napi.h>
#include <uv.h>
#include <node_object_wrap.h>

// nan
#include <napi.h>

// gdal
#include <gdal_priv.h>

#include "group_collection.hpp"
#include "../gdal_mdarray.hpp"

#if GDAL_VERSION_MAJOR > 3 || (GDAL_VERSION_MAJOR == 3 && GDAL_VERSION_MINOR >= 1)

using namespace Napi;
using namespace Napi;

namespace node_gdal {

class GroupArrays : public GroupCollection<GroupArrays, GDALMDArray, GDALGroup, MDArray, Group> {
    public:
  static constexpr const char *_className = "GroupArrays";
  static Napi::FunctionReference constructor;
  static std::shared_ptr<GDALMDArray> __get(std::shared_ptr<GDALGroup> parent, std::string const &name);
  static std::shared_ptr<GDALMDArray> __get(std::shared_ptr<GDALGroup> parent, size_t idx);
  static std::vector<std::string> __getNames(std::shared_ptr<GDALGroup> parent);
  static int __count(std::shared_ptr<GDALGroup> parent);
};

} // namespace node_gdal
#endif
#endif
