#!/bin/bash

pushd `dirname $0` > /dev/null

buildname=`basename $0 .sh`

rm -fr ${buildname}
mkdir -p ${buildname}
cd ${buildname}

cmake -G "Unix Makefiles" ..

popd > /dev/null
