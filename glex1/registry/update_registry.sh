#!/bin/sh

if [[ ($# -ne 1) || (! -d $1) ]]; then
    echo Usage: $0 \<directory\>
    exit 1
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

if [ `find -type d -regex "\./[A-Z0-9]+" | wc -l` -gt 0 ]; then
    # Delete all text files if any non-hidden sub-directory exist.
    rm -fr *.txt doc/
else
    # Sort the specs into directories.
    for f in *.txt; do
        F=`echo $f | tr [a-z] [A-Z]`
        d=`echo $F | sed -r "s/(W?GL[XU]?_)?([A-Z0-9]+).+/\2/"`
        mkdir -p $d
        mv $f $d
    done
fi

popd > /dev/null
