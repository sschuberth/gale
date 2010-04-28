#!/bin/sh

pushd `dirname $0` > /dev/null

find -maxdepth 1 -type d -regex "\./[^\.].+" -exec ./update_registry.sh {} \;

popd > /dev/null
