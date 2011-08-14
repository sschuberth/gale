#!/bin/sh

web=/home/groups/gale/htdocs
#tmp=$web/$(date +"%Y-%b-%d_%H-%M-%S")
tmp=/tmp/gale

# Checkout the whole GALE source code.
git clone git://git.berlios.de/gale $tmp &&

# Remove everything but BerliOS' usage directory.
(cd $web && ls | grep -v '^usage$' | xargs rm -fr) &&

# Generate the HTML documentation via doxygen.
(cd $tmp/gale2/doc && doxygen) &&

# Move the HTML documentation to the htdocs root.
mv $tmp/gale2/doc/html/* $web &&

# Checkout the GeSHi submodule and create symbolic links.
(cd $tmp && git submodule update --init && ln -s ../geshi/geshi-1.0.X/src glex1/geshi && ln -s ../geshi/geshi-1.0.X/src glex2/geshi) &&

# Move GLEX to the htdocs root. 
mv $tmp/glex* $tmp/geshi $web

# Remove the remaining GALE source code.
rm -fr $tmp
