#! /bin/bash

# Build the emulated compiler
docker build -f EmulatedCompiler/Dockerfile -t armv7a_compiler EmulatedCompiler/

# Build raciderry using the emulated compiler
docker run -ti -v $PWD:/root/raciderry armv7a_compiler:latest

# Deploy on remote raspberry
scp Builds/Armv7Makefile/build/raciderry patch@patchbox.local:~