#!/bin/bash

REPORT="report.txt"

#-------------------------------------------------------------------------------

taskset -c 0 ./app > $REPORT
wait
