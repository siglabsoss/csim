#!/bin/bash

#http://stackoverflow.com/questions/59895/can-a-bash-script-tell-what-directory-its-stored-in
THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
CSIM_SRC=$THIS_DIR/../ #Points to root of repo
CSIM_UTEST_BUILD=$CSIM_SRC/../build_utest #define the build tree to be a sibling

mkdir -p $CSIM_UTEST_BUILD

pushd $CSIM_UTEST_BUILD
if ! cmake -D CMAKE_BUILD_TYPE=Debug -DBUILD_UTESTS=true $CSIM_SRC ; then
    EXIT_CODE=$?
    echo "Error building make files for unit tests!"
    exit $EXIT_CODE
fi

if ! make -j3 ; then
    EXIT_CODE=$?
    echo "Error building unit tests!"
    exit $EXIT_CODE
fi
cp -r $CSIM_SRC/../data .

FAILURE_COUNT=0

for filename in ./bin/test_*; do
    echo "Running " $filename
    $filename
    EXIT_CODE=$?
    if [ $EXIT_CODE -ne 0 ]; then
        let "FAILURE_COUNT+=1"
    fi
done
popd

echo "Number of test failures = " $FAILURE_COUNT

exit $FAILURE_COUNT
