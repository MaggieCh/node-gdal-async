# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [3.2.1] 2021-05-24

### Changed
 - Fix a number of memory leaks (exception throwing paths in all `gdal.SpatialReference.from*`, `gdal.DatasetBands.create`)

## [3.2.0] 2021-05-21

### Added
 - GDAL 3.2.3
 - Built-in vsicurl and major cloud services (Amazon, Google, Microsoft and Alibaba) support on all platforms
 - Support async_hooks
 - Support progress callbacks
 - Support different algorithms when resampling on-the-fly
 - Add `gdal.Geometry.fromGeoJsonBuffer(Async)()`, a completely parallelizable alternative to `gdal.Geometry.fromGeoJson()` for importing large GeoJSON geometries without blocking the event loop
 - Add `gdal.LayerFeatures.setAsync()`

### Changed
 - `gdal.Geometry.fromGeoJson` now throws an `Error` on error instead of simply returning a null geometry 
   
   Previously all `gdal.Geometry.from*` functions except `fromGeoJson` threw undocumented errors, now all of them, including `fromGeoJson` throw documented errors

## [3.1.1] 2021-05-14

### Changed
  - Fully unit-tested TypeScript definitions with many method signatures fixed
  - Properties without setters are now correctly identified as being read-only in the TypeScript bindings
  - Exported all previously internal `_options` types used for methods such as `RasterBand.read` or `reprojectImage`

## [3.1.0] 2021-05-11

### Added
  - Add full TypeScript support
  
  - Complete reworking of the Geometry code adding support for CircularString, CompoundCurve and MultiCurve
  - Add JS iterators to all collections
  - Enable MapInfo TAB and MIF/MID
  - Fixed a typo in `gdal.SpatialReference`, `isVectical()` method should be `isVertical()`, the misspelled method will be kept until the next major release
  - Expanded unit testing, code coverage is 81% up from 70%

### Changed
  - Process the unit testing suite with ASAN and fix all memory leaks

### Added
  - Support Node.js 16
  - Merge `node-gdal-next@2.8.0`

## Changed
  - Build GDAL against the internal zlib in Node.js to avoid a conflict causing data corruption
  - Enforce code coverage checks and re-enable all disabled unit tests

## [3.0.1] 2021-04-09

### Changed

  - Follow the Node.js callback error convention, returning `null` instead of `undefined` for the `error` argument on success and returning an `Error` object instead of a string on error
  - Lots of minor documentation fixes

# [3.0.0] 2021-03-26

First release of `node-gdal-async`, forked from `node-gdal-next@2.6.2`

### Added
  - **Full support for asynchronous operations**
  - GDAL 3.2.2
  - vsimem support
  - MVT writing
  - SQLite integration

### Changed
  - Packages built through Github Actions
  - Packages published through Github Packages
  - Numerous small bugfixes
