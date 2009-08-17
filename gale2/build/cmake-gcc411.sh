#!/bin/sh

pushd `dirname $0`/../glex > /dev/null

# Download and parse the OpenGL extension registry if needed.
if [ ! -e "../../glex/registry/OpenGL.org/ARB/color_buffer_float.txt" ]; then
    echo "*** Generating local OpenGL registry ..."
    ../../glex/registry/update_registry.sh ../../glex/registry/OpenGL.org
fi

# List the extensions used in the project.
echo "*** Parsing required OpenGL extensions ..."
while read e; do
    echo "Parsing file \"${e}\" ..."
    ../../glex/glex.sh spec=../../glex/registry/OpenGL.org/${e}
done < ../build/opengl-extensions.txt

popd > /dev/null

pushd `dirname $0` > /dev/null

buildname=gcc41

rm -fr ${buildname}
mkdir -p ${buildname}
cd ${buildname}

cmake -G "Unix Makefiles" ..

popd > /dev/null
