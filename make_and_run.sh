#!/bin/bash

NJOBS=4
if [ "$1" != "" ] ; then
    NJOBS=$1
fi

rm ashell
mkdir -p build && cd build
cmake .. \
    && make -j $NJOBS \
    && cp ashell ..\
    && cd ..
echo "Running ashell"
./ashell
