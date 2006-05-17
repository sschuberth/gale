#!/bin/bash
svn stat -v $1 | sed -nr 's/^[ A-Z?\*|!]{1,15}([0-9]+) +([0-9]+).*/Working revision \1 \/ commited revision \2/p'
