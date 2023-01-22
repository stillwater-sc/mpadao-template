#!/usr/bin/env bash

# script to generate the docker build containers with specific compilers installed
# precondition: successful docker login so that the docker push can succeed

# GCC compiler containers
docker build --target gcc12builder -t stillwater/mpadao:gcc12builder -f Dockerfile.gcc12builder .
#docker push stillwater/mpadao:gcc12builder

