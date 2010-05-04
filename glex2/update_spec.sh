#!/bin/sh

pushd `dirname $0` > /dev/null

wget --timestamping --directory-prefix=spec \
    http://www.opengl.org/registry/api/enumext.spec \
    http://www.opengl.org/registry/api/gl.spec \
    http://www.opengl.org/registry/api/gl.tm \
    http://www.opengl.org/registry/api/wglenumext.spec \
    http://www.opengl.org/registry/api/wgl.spec \
    http://www.opengl.org/registry/api/wgl.tm

popd > /dev/null
