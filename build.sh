#!/usr/bin/env bash

rm -rf ./build

if [[ $1 == "-o" ]]; then
  echo "-o flag found, not updating external deps"
  cmake -DBUILD_OFFLINE=ON -S . -B ./build
else
  cmake -S . -B ./build
fi

cmake --build ./build

rm compile_commands.json

ln ./build/compile_commands.json .
