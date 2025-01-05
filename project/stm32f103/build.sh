#!/bin/sh

cmake -B build -S . -G Ninja --fresh
cmake --build build