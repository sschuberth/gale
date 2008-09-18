#!/bin/sh

svn stat -v $1 | sed -nr 's/^[ A-Z?\*|!]{1,15}([0-9]+) +([0-9]+).*/Committed revision is \2, working revision is \1/p'
