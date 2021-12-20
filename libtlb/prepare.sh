#!/bin/bash

cd $(dirname $0)

declare -a MODULES=("pmc_module")

for MODULE in ${MODULES[@]}; do
  if [ -z $(lsmod | grep $MODULE -o) ]; then
    echo "Building and installing kernel module: $MODULE."

    cd  ./$MODULE

    make clean
    make

    ./install.sh

    echo "-------------------------------"
    lsmod | grep $MODULE
    echo "-------------------------------"

    cd ..
  fi
done
