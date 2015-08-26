#!/bin/bash

./opengl-parse.sh

pushd $(dirname $0) > /dev/null

buildname=GCC

rm -fr $buildname
mkdir -p $buildname
cd $buildname

cmake --version
cmake -G "Unix Makefiles" ..

popd > /dev/null
