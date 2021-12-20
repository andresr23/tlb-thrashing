#!/bin/bash

LIBTLB_PATH="../../libtlb"

PLATFORM=${1:-"skylake"}

OUTPUT_0="rate.dat"
OUTPUT_1="latex.dat"

FINALF_0="rate-$PLATFORM.dat"
FINALF_1="latex-$PLATFORM.dat"

REPORT="report-$PLATFORM.txt"
PLOT_FILE="${PWD##*/}-$PLATFORM.png"

#-------------------------------------------------------------------------------

./$LIBTLB_PATH/prepare.sh
wait

taskset -c 0 ./app > $REPORT
wait

mv -f $OUTPUT_0 $FINALF_0
mv -f $OUTPUT_1 $FINALF_1

gnuplot -e "data_file='$FINALF_0'" -e "plot_file='$PLOT_FILE'" plot.gp
