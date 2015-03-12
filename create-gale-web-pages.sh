#!/bin/sh

if [ $# -ne 2 ]; then
    echo "Usage: $(basename $0) <git repository url> <web page directory>"
    exit 1
fi

repo=$1
web=$2
tmp=/tmp/gale

# Checkout the whole GALE source code.
git clone $repo $tmp &&

# Remove everything from the web page.
mkdir -p $web && rm -fr $web/* &&

# Generate the HTML documentation via doxygen.
(cd $tmp/gale2/doc && doxygen) &&

# Move the HTML documentation to the web page.
mv $tmp/gale2/doc/html/* $web &&

# Checkout the GeSHi submodule and create symbolic links.
(cd $tmp && git submodule update --init && ln -s ../geshi/geshi-1.0.X/src glex1/geshi && ln -s ../geshi/geshi-1.0.X/src glex2/geshi) &&

# Move GLEX to the web page.
mv $tmp/glex* $tmp/geshi $web

# Remove the remaining GALE source code.
rm -fr $tmp
