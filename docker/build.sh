#!/usr/bin/env bash
set -euo pipefail

# Build (and optionally push) the MPADAO development container.
# Usage:
#   ./docker/build.sh          # build only
#   ./docker/build.sh --push   # build and push to Docker Hub

IMAGE="stillwater/mpadao:latest"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

docker build -t "$IMAGE" "$SCRIPT_DIR"

if [[ "${1:-}" == "--push" ]]; then
    docker push "$IMAGE"
fi
