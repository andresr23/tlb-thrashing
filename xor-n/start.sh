#!/bin/bash

N=${1:-4}
EXPERIMENT="${PWD##*/}"

OUTPUT="xor.dat"
REPORT="report-$N.txt"

#-------------------------------------------------------------------------------

./app $1 > $REPORT
wait

gnuplot -e "n='$N'" plot.gp
wait

rm -f $OUTPUT
