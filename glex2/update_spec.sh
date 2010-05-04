#!/bin/sh

wget --timestamping --directory-prefix=spec \
    http://www.opengl.org/registry/api/enumext.spec \
    http://www.opengl.org/registry/api/gl.spec \
    http://www.opengl.org/registry/api/gl.tm
