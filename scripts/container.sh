#!/bin/sh

OP=$1
DIST=$2
NODEJS=$3
GDAL=$4
if [ -z "${OP}" ]; then
  echo "container.sh <release|dev|shell> [ubuntu:{18.04|20.04|21.10}|centos:stream8|fedora:{33|34}|debian:{buster|bullseye}|amazonlinux] [<Node.js version>] [shared]"
  exit 1
fi
if [ ! -d "test/platforms" ]; then
  echo "This command must be run from the node-gdal-async root directory"
  exit 1
fi

if [ -z "${DIST}" ]; then
  DIST=ubuntu
  VERSION=16.04
else
  VERSION=`echo ${DIST} | cut -f 2 -d":"`
  DIST=`echo ${DIST} | cut -f 1 -d":"`
fi
if [ -z "${NODEJS}" ]; then
  NODEJS=lts
fi
if [ -z "${GDAL}" ]; then
  GDAL="bundled"
fi

CONTAINER="${DIST}-node-gdal:${VERSION}_${NODEJS}_${GDAL}"
SEP="\n=======================================================\n"
echo -e "${SEP}Building container ${CONTAINER}${SEP}"

docker build \
        --build-arg VERSION=${VERSION} --build-arg NODEJS=${NODEJS} --build-arg GDAL=${GDAL} \
        -t ${CONTAINER} -f test/platforms/Dockerfile.${DIST} test/platforms

COMMON_ARGS="--user `id -u`:`id -g` --env MOCHA_TEST_NETWORK -v `pwd`:/src:ro"

echo "Checking for ccache"
if which ccache; then
  CCACHE_DIR=`ccache --get-config=cache_dir`
  if [ ! -w ${CCACHE_DIR} ]; then
    echo "${CCACHE_DIR} not available, creating it"
    mkdir -p ${CCACHE_DIR}
  fi
  echo "cache in ${CCACHE_DIR}"
  COMMON_ARGS="${COMMON_ARGS} -v ${CCACHE_DIR}:/ccache --env CCACHE_DIR=/ccache"
fi

case ${OP} in
  release)
    echo -e "${SEP}Testing in ${CONTAINER}${SEP}"
    docker run ${COMMON_ARGS} ${CONTAINER} RELEASE || exit 1
    ;;
  dev)
    echo -e "${SEP}Testing in ${CONTAINER}${SEP}"
    docker run ${COMMON_ARGS} ${CONTAINER} DEV || exit 1
    ;;
  publish)
    echo -e "${SEP}Publishing in ${CONTAINER}${SEP}"
    docker run ${COMMON_ARGS} --env NODE_PRE_GYP_GITHUB_TOKEN PUBLISH ${CONTAINER} || exit 1
    ;;
  shell)
    echo -e "${SEP}Testing in ${CONTAINER} and running a shell${SEP}"
    docker run -it ${COMMON_ARGS} ${CONTAINER} DEV /bin/bash
    ;;
  asan)
    echo -e "${SEP}Testing w/asan in ${CONTAINER}${SEP}"
    docker run ${COMMON_ARGS} ${CONTAINER} DEV || exit 1
    ;;
esac
