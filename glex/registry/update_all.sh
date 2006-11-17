#!/bin/bash

find -type d -regex "\./[^\.].+" -maxdepth 1 -exec update_registry.sh {} \;
