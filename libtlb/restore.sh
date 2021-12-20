#!/bin/bash

cd $(dirname $0)

declare -a MODULES=("pmc_module")

for MODULE in ${MODULES[@]}; do
  if [ $(lsmod | grep $MODULE -o) ]; then
    echo "Uninstalling kernel module: $MODULE."

    ./$MODULE/uninstall.sh
  fi

  cd $MODULE
  make clean
  cd ..
done
