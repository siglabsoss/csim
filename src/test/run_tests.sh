#!/bin/bash

#http://stackoverflow.com/questions/59895/can-a-bash-script-tell-what-directory-its-stored-in
THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
CSIM_SRC=$THIS_DIR/../../ #Points to root of repo
CSIM_UTEST_BUILD=$CSIM_SRC/../csim_utest_build #define the build tree to be a sibling

#Construct semicolon separated list of unit test source files relative to this directory
#because that's where the CMakeLists.txt file lives.
#XXX as the number of files grows, this logic may need to be udpated because the maximum
#number of command line arguments could be exhausted although xargs supposedly handles
#that

if [ $# -eq 0 ]; then
    TEST_FILTER=utests/.*cpp
else
    TEST_FILTER=$1
fi

pushd $THIS_DIR
export UNIT_TEST_SRCS=`find ../ | grep $TEST_FILTER | xargs echo |  sed 's/ /;/g'`
popd

mkdir -p $CSIM_UTEST_BUILD

pushd $CSIM_UTEST_BUILD
cmake -D CMAKE_BUILD_TYPE=Debug $CSIM_SRC
make -j utests
cp -r $CSIM_SRC/data .
./bin/utests
EXIT_CODE=$?
popd

exit $EXIT_CODE
