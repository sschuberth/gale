#!/bin/sh

git rev-list --max-count=1 HEAD -- $1 | sed -rn "s/^([0-9a-f]{7}).+/Last modified in \1/p"
