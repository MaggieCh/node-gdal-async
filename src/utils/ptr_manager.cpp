#include "ptr_manager.hpp"
#include "../gdal_dataset.hpp"
#include "../gdal_layer.hpp"
#include "../gdal_rasterband.hpp"

#include <sstream>

namespace node_gdal {

PtrManager::PtrManager() : uid(1), layers(), bands(), datasets() {
  uv_mutex_init(&master_lock);
}

PtrManager::~PtrManager() {
}

void PtrManager::lock() {
  uv_mutex_lock(&master_lock);
}

void PtrManager::unlock() {
  uv_mutex_unlock(&master_lock);
}

bool PtrManager::isAlive(long uid) {
  if (uid == 0) return true;
  return bands.count(uid) > 0 || layers.count(uid) > 0 || datasets.count(uid) > 0;
}

uv_mutex_t *PtrManager::tryLockDataset(long uid) {
  lock();
  auto parent = datasets.find(uid);
  if (parent != datasets.end()) {
    uv_mutex_lock(parent->second->async_lock);
    unlock();
    return parent->second->async_lock;
  }
  unlock();
  return nullptr;
}

long PtrManager::add(OGRLayer *ptr, long parent_uid, bool is_result_set) {
  lock();
  PtrManagerLayerItem *item = new PtrManagerLayerItem();
  item->uid = uid++;
  item->parent = datasets[parent_uid];
  item->ptr = ptr;
  item->is_result_set = is_result_set;
  layers[item->uid] = item;

  PtrManagerDatasetItem *parent = datasets[parent_uid];
  parent->layers.push_back(item);
  unlock();
  return item->uid;
}

long PtrManager::add(GDALRasterBand *ptr, long parent_uid) {
  PtrManagerRasterBandItem *item = new PtrManagerRasterBandItem();
  item->uid = uid++;
  item->parent = datasets[parent_uid];
  item->ptr = ptr;
  bands[item->uid] = item;

  PtrManagerDatasetItem *parent = datasets[parent_uid];
  parent->bands.push_back(item);
  return item->uid;
}

long PtrManager::add(GDALDataset *ptr, uv_mutex_t *async_lock) {
  lock();
  PtrManagerDatasetItem *item = new PtrManagerDatasetItem();
  item->uid = uid++;
  item->ptr = ptr;
  item->async_lock = async_lock;
  datasets[item->uid] = item;
  unlock();
  return item->uid;
}

#if GDAL_VERSION_MAJOR < 2
long PtrManager::add(OGRDataSource *ptr) {
  PtrManagerDatasetItem *item = new PtrManagerDatasetItem();
  item->uid = uid++;
  item->ptr_datasource = ptr;
  datasets[item->uid] = item;
  return item->uid;
}
#endif

void PtrManager::dispose(long uid) {
  if (datasets.count(uid))
    dispose(datasets[uid]);
  else if (layers.count(uid))
    dispose(layers[uid]);
  else if (bands.count(uid))
    dispose(bands[uid]);
}

void PtrManager::dispose(PtrManagerDatasetItem *item) {
  lock();
  uv_mutex_lock(item->async_lock);
  datasets.erase(item->uid);
  uv_mutex_unlock(item->async_lock);
  unlock();

  while (!item->layers.empty()) { dispose(item->layers.back()); }
  while (!item->bands.empty()) { dispose(item->bands.back()); }

#if GDAL_VERSION_MAJOR < 2
  if (item->ptr_datasource) {
    Dataset::datasource_cache.erase(item->ptr_datasource);
    OGRDataSource::DestroyDataSource(item->ptr_datasource);
  }
#endif
  if (item->async_lock) {
    uv_mutex_destroy(item->async_lock);
    delete item->async_lock;
  }
  if (item->ptr) {
    Dataset::dataset_cache.erase(item->ptr);
    GDALClose(item->ptr);
  }

  delete item;
}

void PtrManager::dispose(PtrManagerRasterBandItem *item) {
  uv_mutex_t *async_lock = tryLockDataset(item->parent->uid);
  RasterBand::cache.erase(item->ptr);
  bands.erase(item->uid);
  item->parent->bands.remove(item);
  if (async_lock != nullptr) uv_mutex_unlock(async_lock);
  delete item;
}

void PtrManager::dispose(PtrManagerLayerItem *item) {
  uv_mutex_t *async_lock = tryLockDataset(item->parent->uid);
  Layer::cache.erase(item->ptr);
  layers.erase(item->uid);
  item->parent->layers.remove(item);

#if GDAL_VERSION_MAJOR < 2
  OGRDataSource *parent_ds = item->parent->ptr_datasource;
#else
  GDALDataset *parent_ds = item->parent->ptr;
#endif

  if (item->is_result_set) { parent_ds->ReleaseResultSet(item->ptr); }
  if (async_lock != nullptr) uv_mutex_unlock(async_lock);

  delete item;
}

} // namespace node_gdal