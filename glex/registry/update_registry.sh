#!/bin/bash

if [ $# -ne 1 -o ! -d $1 ]; then
    echo Usage: $0 \<directory\>
    exit -1
fi

pushd $1 > /dev/null

# Download the specs from the Internet.
wget --execute="robots=off" \
     --timestamping         \
     --recursive            \
     --level 2              \
     --no-parent            \
     --no-host-directories  \
     --accept=txt           \
     `sed -rn "s/; wget=(.+)/\1/p" *.url` `sed -rn "s/URL=(.+)/\1/p" *.url`

#TODO: Delete *.txt files if any non-hidden sub-directory exists.
#rm -f *.txt

# Sort the specs into directories.
for f in *.txt; do
    F=`echo $f | tr [a-z] [A-Z]`
    d=`echo $F | sed -r "s/(W?GL[XU]?_)?([A-Z0-9]+).+/\2/"`
    mkdir -p $d
    mv $f $d
done

popd > /dev/null
