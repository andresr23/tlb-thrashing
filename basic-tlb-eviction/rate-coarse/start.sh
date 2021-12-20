#!/bin/bash

LIBTLB_PATH="../../libtlb"

PLATFORM=${1:-"skylake"}

OUTPUT="rate.dat"
REPORT="report-$PLATFORM.txt"

DATA_FILE="rate-$PLATFORM.dat"
PLOT_FILE_0="${PWD##*/}-$PLATFORM.png"
PLOT_FILE_1="${PWD##*/}-matrix-$PLATFORM.png"

#-------------------------------------------------------------------------------

./$LIBTLB_PATH/prepare.sh
wait

taskset -c 0 ./app > $REPORT
wait

mv -f $OUTPUT $DATA_FILE

gnuplot -e "data_file='$DATA_FILE'"     \
        -e "plot_file_0='$PLOT_FILE_0'" \
        -e "plot_file_1='$PLOT_FILE_1'" \
        plot.gp
