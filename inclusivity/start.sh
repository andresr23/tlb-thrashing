#!/bin/bash

LIBTLB_PATH="../libtlb"

PLATFORM=${1:-"skylake"}

REPORT="report-$PLATFORM.dat"

#-------------------------------------------------------------------------------

./$LIBTLB_PATH/prepare.sh
wait

taskset -c 0 ./app > $REPORT
wait
