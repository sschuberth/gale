#!/bin/sh

pushd $(dirname $0) > /dev/null

urls="
    http://www.opengl.org/registry/api/enumext.spec
    http://www.opengl.org/registry/api/gl.spec
    http://www.opengl.org/registry/api/gl.tm
    http://www.opengl.org/registry/api/wglenumext.spec
    http://www.opengl.org/registry/api/wglext.spec
    http://www.opengl.org/registry/api/wgl.tm
"

if [ -n "$(wget --version 2> /dev/null)" ]; then
    wget --timestamping --directory-prefix=spec $urls
elif [ -n "$(curl --version 2> /dev/null)" ]; then
    mkdir spec 2> /dev/null
    cd spec
    for i in $urls; do
        echo Updating $i
        curl --remote-name --remote-time --time-cond $(basename $i) $i
    done
fi

popd > /dev/null
