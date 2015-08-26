#!/bin/bash

pushd $(dirname $0) > /dev/null

urls="
    https://www.opengl.org/registry/oldspecs/enumext.spec
    https://www.opengl.org/registry/oldspecs/gl.spec
    https://www.opengl.org/registry/oldspecs/gl.tm
    https://www.opengl.org/registry/oldspecs/wglenumext.spec
    https://www.opengl.org/registry/oldspecs/wglext.spec
    https://www.opengl.org/registry/oldspecs/wgl.tm
"

if type -p wget > /dev/null; then
    wget --timestamping --directory-prefix=spec $urls
elif type -p curl > /dev/null; then
    mkdir -p spec
    cd spec
    for i in $urls; do
        echo "Updating $i"
        curl --location --remote-name --remote-time --time-cond $(basename $i) $i
    done
else
    echo "Error: Neither Wget nor cURL was found."
fi

popd > /dev/null
