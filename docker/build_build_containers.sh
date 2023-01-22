#!/usr/bin/env bash

# script to generate the docker build containers with specific compilers installed
# precondition: successful docker login so that the docker push can succeed

# GCC compiler containers
docker build --target gcc9builder -t stillwater/mpadao:gcc9builder -f Dockerfile.gcc9builder .
docker push stillwater/mpadao:gcc9builder
docker build --target gcc10builder -t stillwater/mpadao:gcc10builder -f Dockerfile.gcc10builder .
docker push stillwater/mpadao:gcc10builder
docker build --target gcc11builder -t stillwater/mpadao:gcc11builder -f Dockerfile.gcc11builder .
docker push stillwater/mpadao:gcc11builder
docker build --target gcc12builder -t stillwater/mpadao:gcc12builder -f Dockerfile.gcc12builder .
docker push stillwater/mpadao:gcc12builder

# CLang compiler containers
docker build --target clang11builder -t stillwater/mpadao:clang11builder -f Dockerfile.clang11builder .
docker push stillwater/mpadao:clang11builder
docker build --target clang12builder -t stillwater/mpadao:clang12builder -f Dockerfile.clang12builder .
docker push stillwater/mpadao:clang12builder
docker build --target clang13builder -t stillwater/mpadao:clang13builder -f Dockerfile.clang13builder .
docker push stillwater/mpadao:clang13builder
docker build --target clang14builder -t stillwater/mpadao:clang14builder -f Dockerfile.clang14builder .
docker push stillwater/mpadao:clang14builder
#docker build --target clang15builder -t stillwater/mpadao:clang15builder -f Dockerfile.clang15builder .
#docker push stillwater/mpadao:clang15builder
