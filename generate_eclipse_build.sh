#!/bin/bash

#Simple script which invokes cmake in a separate build directory that sits beside the source tree

#http://stackoverflow.com/questions/59895/can-a-bash-script-tell-what-directory-its-stored-in
THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
CSIM_SRC=$THIS_DIR #source tree root is where this script sits
CSIM_BUILD=$CSIM_SRC/../csim_build #define the build tree to be a sibling

mkdir -p $CSIM_BUILD

pushd $CSIM_BUILD
cmake -G"Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug $CSIM_SRC
popd

