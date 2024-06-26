# Copyright 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002,
# 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014,
# 2015, 2016, 2017, 2018
# University Corporation for Atmospheric Research/Unidata.

# See netcdf-c/COPYRIGHT file for more info.
SET(dap4_SOURCES d4curlfunctions.c d4fix.c d4data.c d4file.c d4parser.c d4meta.c d4varx.c d4dump.c d4swap.c d4chunk.c d4printer.c d4read.c d4http.c d4util.c d4odom.c d4cvt.c d4debug.c ncd4dispatch.c)

add_library(dap4 OBJECT ${dap4_SOURCES})

###
# Options related to the man page generation.
###

###
# The C API man page.
###

set(man_MANS netcdf.3)

set(ARGS_MANPAGE -DAPI=C)

IF (USE_HDF5)
  SET(ARGS_MANPAGE ${ARGS_MANPAGE} -DNETCDF4=TRUE)
ENDIF(USE_HDF5)

IF (BUILD_DAP)
  SET(ARGS_MANPAGE ${ARGS_MANPAGE} -DDAP=TRUE)
ENDIF(BUILD_DAP)

IF (BUILD_PARALLEL)
  SET(ARGS_MANPAGE ${ARGS_MANPAGE} -DPARALLEL_IO=TRUE)
ENDIF(BUILD_PARALLEL)

FILE(GLOB CUR_EXTRA_DIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
SET(CUR_EXTRA_DIST ${CUR_EXTRA_DIST} dce.y CMakeLists.txt Makefile.am)
ADD_EXTRA_DIST("${CUR_EXTRA_DIST}")
