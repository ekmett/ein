#!/bin/bash

# run as the current user not root to more closely emulate the runner environment
export ACT_USER_UID=$(id -u)
export ACT_USER_GID=$(id -g)

# Check if --bind is among the arguments
if [[ " $* " == *" --bind "* ]]; then
  # Run act in the background with nohup and capture the process ID
  nohup act $@ & ACT_PID=$!

  # Wait briefly for the container to start
  sleep 2

  # Find the container ID of the running act container
  CONTAINER_ID=$(docker ps --filter "ancestor=catthehacker/ubuntu:act-latest" --format "{{.ID}}")

  # Check if the container ID was found
  if [ -z "$CONTAINER_ID" ]; then
    echo "No act container found."
    exit 1
  fi

  echo "Act container ID: $CONTAINER_ID"

  # Attach to the container for interactive debugging
  docker exec -it "$CONTAINER_ID" /bin/bash

  # Once done, wait for act to complete, then stop and remove the container
  wait $ACT_PID
  docker stop "$CONTAINER_ID"
  docker rm "$CONTAINER_ID"

else
  # If --bind is not among the arguments, delegate to act directly
  exec act $@
fi

