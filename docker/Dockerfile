#
# Single development container for the MPADAO template
#
# Build:
#   docker build -t stillwater/mpadao:latest docker/
#
# Run (mount repo):
#   docker run --rm -it -v $(pwd):/home/dev/mpadao stillwater/mpadao:latest
#
# Use Clang instead of the default GCC:
#   CC=clang-18 CXX=clang++-18 cmake ..
#

FROM ubuntu:24.04

LABEL maintainer="Stillwater Supercomputing, Inc."
LABEL description="MPADAO development container with GCC 14 and Clang 18"

# Avoid interactive prompts during package install
ENV DEBIAN_FRONTEND=noninteractive

# hadolint ignore=DL3008
RUN rm -f /etc/apt/apt.conf.d/docker-clean \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
       gcc-14 \
       g++-14 \
       clang-18 \
       cmake \
       make \
       git \
       libboost-dev \
    && rm -rf /var/lib/apt/lists/*

# Set GCC 14 as the default compiler
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 \
        --slave /usr/bin/g++ g++ /usr/bin/g++-14 \
    && update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-14 100 \
    && update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-14 100

# Non-root user (remove default ubuntu user so dev gets UID 1000)
RUN userdel -r ubuntu 2>/dev/null || true \
    && useradd -ms /bin/bash -u 1000 dev
USER dev
WORKDIR /home/dev

CMD ["/bin/bash"]
