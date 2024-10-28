#!/bin/bash

set -e

hash=$(sha256sum Dockerfile | awk '{print substr($1, 1, 12)}')

# root dockerfile
image="ghcr.io/ekmett/ein:${hash}"
if ! docker manifest inspect "$image" &> /dev/null; then
  echo "Image $image not found. Building and pushing..."
  docker build --build-arg HOST_UID="$uid" -t "$image" -f Dockerfile --progress=plain .
  docker push "$image"
else
  echo "Image $image exists."
fi

uid=$(id -u)

# restricted dockerfile that runs as local user
image="ghcr.io/ekmett/ein:${hash}-local-$uid"
if ! docker manifest inspect "$image" &> /dev/null; then
  echo "Image $image not found. Building and pushing..."
  docker build --build-arg HASH=$hash --build-arg HOST_UID="$uid" -t "$image" -f Dockerfile.local .
  docker push "$image"
else
  echo "Image $image exists."
fi
