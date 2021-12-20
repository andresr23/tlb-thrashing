#!/bin/bash

LIBTLB_PATH="../../libtlb"

PLATFORM=${1:-"skylake"}

OUTPUT="rate.dat"
FINALF="rate-$PLATFORM.dat"

REPORT="report-$PLATFORM.txt"
PLOT_FILE="${PWD##*/}-$PLATFORM.png"

#-------------------------------------------------------------------------------

./$LIBTLB_PATH/prepare.sh
wait

taskset -c 0 ./app > $REPORT
wait

mv -f $OUTPUT $FINALF

gnuplot -e "data_file='$FINALF'" -e "plot_file='$PLOT_FILE'" plot.gp
