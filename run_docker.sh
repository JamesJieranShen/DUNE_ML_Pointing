docker run --rm -it --init \
  --ipc=host \
  --user="$(id -u):$(id -g)" \
  --volume="$PWD:/workspace" \
  --network="host" \
  pytorch jupyter notebook