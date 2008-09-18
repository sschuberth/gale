#!/bin/sh

find -type d -regex "\./[^\.].+" -maxdepth 1 -exec update_registry.sh {} \;
