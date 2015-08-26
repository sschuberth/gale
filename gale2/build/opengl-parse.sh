#!/bin/bash

pushd $(dirname $0)/../glex > /dev/null

echo "*** Deleting existing OpenGL API initialization files ..."
rm GLEX_*.*

echo "*** Updating OpenGL specification files ..."
../../glex2/update_spec.sh

echo "*** Parsing required OpenGL APIs ..."
../../glex2/glex.sh es=../../glex2/spec/enumext.spec fs=../../glex2/spec/gl.spec tm=../../glex2/spec/gl.tm api=@../build/opengl-apis.txt
../../glex2/glex.sh es=../../glex2/spec/wglenumext.spec fs=../../glex2/spec/wglext.spec tm=../../glex2/spec/wgl.tm api=@../build/opengl-apis.txt

popd > /dev/null
