#!/bin/sh

find -maxdepth 1 -type d -regex "\./[^\.].+" -exec update_registry.sh {} \;
