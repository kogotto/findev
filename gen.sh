#!/bin/bash

source defs.sh

cmake -S . -B $BUILD_DIR $OPTIONS -DCMAKE_BUILD_TYPE=Debug;
ln -sf $BUILD_DIR/compile_commands.json .;
