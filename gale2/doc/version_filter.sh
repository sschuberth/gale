#!/bin/sh

sed_version=$(sed --version 2> /dev/null)
if [[ $? == 0 && $sed_version == GNU* ]]; then
    sed_options="-rn"
else
    sed_options="-En"
fi

git rev-list --max-count=1 HEAD -- $1 | sed $sed_options "s/^([0-9a-f]{7}).+/Last modified in \1/p"
