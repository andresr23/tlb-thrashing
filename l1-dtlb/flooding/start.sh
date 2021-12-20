#!/bin/bash

LIBTLB_PATH="../../libtlb"

PLATFORM=${1:-"skylake"}

OUTPUT_0="rate.dat"
OUTPUT_1="sets.dat"

FINALF_0="rate-$PLATFORM.dat"
FINALF_1="sets-$PLATFORM.dat"

REPORT="report-$PLATFORM.txt"
PLOT_FILE="${PWD##*/}-$PLATFORM.png"

#-------------------------------------------------------------------------------

./$LIBTLB_PATH/prepare.sh
wait

taskset -c 0 ./app > $REPORT
wait

mv -f $OUTPUT_0 $FINALF_0
mv -f $OUTPUT_1 $FINALF_1

gnuplot -e "data_file_0='$FINALF_0'" \
        -e "data_file_1='$FINALF_1'" \
        -e "plot_file='$PLOT_FILE'"  \
        plot.gp
