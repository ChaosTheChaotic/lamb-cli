#!/usr/bin/env bash

rm -rf ./build

cmake -S . -B ./build
cmake --build ./build

rm compile_commands.json

ln ./build/compile_commands.json .
