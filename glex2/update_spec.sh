#!/bin/sh

pushd $(dirname $0) > /dev/null

urls="
    https://www.opengl.org/registry/api/enumext.spec
    https://www.opengl.org/registry/api/gl.spec
    https://www.opengl.org/registry/api/gl.tm
    https://www.opengl.org/registry/api/wglenumext.spec
    https://www.opengl.org/registry/api/wglext.spec
    https://www.opengl.org/registry/api/wgl.tm
"

if [ -n "$(wget --version 2> /dev/null)" ]; then
    wget --timestamping --directory-prefix=spec $urls
elif [ -n "$(curl --version 2> /dev/null)" ]; then
    mkdir spec 2> /dev/null
    cd spec
    for i in $urls; do
        echo "Updating $i"
        curl --location --remote-name --remote-time --time-cond $(basename $i) $i
    done
else
    echo "Error: Neither Wget nor cURL was found."
fi

popd > /dev/null
