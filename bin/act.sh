#!/bin/bash

# Define the custom image name
# IMAGE_NAME="ekmett/ein:ubuntu22-04"
IMAGE_NAME="catthehacker/ubuntu:act-latest"

# Check if the custom Docker image exists
if ! docker images --format "{{.Repository}}" | grep -q "^${IMAGE_NAME}$"; then
  echo "Image ${IMAGE_NAME} not found. Building it now..."



  # let me be sloppy with host vs. device
  export HOST_HOME=${HOME}
  export HOST_UID=$(id -u)

  cat Dockerfile | docker build --build-arg HOST_UID=$(id -u) -t "${IMAGE}" -f - -
EOF

  echo "Image ${IMAGE_NAME} built successfully."
fi

# Proceed with the existing logic to check for --bind
if [[ " $* " == *" --bind "* ]]; then
  # Run act with the custom Docker image in the background, capture process ID
  echo "Running \`act $@\` in the background on ${IMAGE_NAME}..."
  nohup act --pull=false -P "ubuntu-latest=${IMAGE_NAME}" $@ & ACT_PID=$!

  # Wait briefly for the container to start
  sleep 2

  # Find the container ID of the running act container
  CONTAINER_ID=$(docker ps --filter "ancestor=${IMAGE_NAME}" --format "{{.ID}}")

  # Check if the container ID was found
  if [ -z "$CONTAINER_ID" ]; then
    echo "No act container found. Exiting."
    exit 1
  fi

  echo "Container $CONTAINER_ID act (PID $ACT_PID) running with container ID: $CONTAINER_ID"

  # Attach to the container for interactive debugging
  docker exec -it "$CONTAINER_ID" /bin/bash

  # Once done, wait for act to complete, then stop and remove the container
  echo -n "Waiting for act (PID $ACT_PID) to finish..."
  wait $ACT_PID
  echo " done."

  echo -n "Stopping container..."
  docker stop "$CONTAINER_ID" > /dev/null
  echo " stopped."

  echo -n "Removing container..."
  docker rm "$CONTAINER_ID" > /dev/null
  echo " removed."

else
  # If --bind is not among the arguments, run act directly with the custom image
  echo "Running act directly without --bind."
  exec act --pull=false -P "ubuntu-latest=${IMAGE_NAME}" $@
fi
