set terminal png font 'Helvetica, 19' size 1620,540 enhanced
if(!exists('data_file')) data_file = 'rate-skylake.dat'
if(!exists('plot_file_0')) plot_file_0 = 'plot-skylake.png'
if(!exists('plot_file_1')) plot_file_1 = 'matrix-skylake.png'

unset title

# Plot. ------------------------------------------------------------------------
set output plot_file_0

xtics_min = 64
xtics_max = 3072
xtics_inc = 64

xrange_min = xtics_min - xtics_inc
xrange_max = xtics_max + xtics_inc

ytics_min = 0
ytics_max = 1
ytics_inc = 0.1

yrange_min = ytics_min - ytics_inc
yrange_max = ytics_max + ytics_inc

set xrange[xrange_min:xrange_max]
set xtics xtics_min, xtics_inc, xtics_max font ',10' rotate
set xlabel '| PTE Set |' font ',12' offset 0,1.5

set yrange[yrange_min:yrange_max]
set ytics ytics_min, ytics_inc, ytics_max font ',10'
set ylabel 'TLB Eviction Rate' font ',12' offset 2,0

set grid

set key left top box font ',11'

plot data_file u (($0 + 1) * xtics_inc):($1) with linespoints lc 'red'  lw 1 lt 1 pt 7 t 'Non-aligned', \
     data_file u (($0 + 1) * xtics_inc):($8) with linespoints lc 'blue' lw 1 lt 0 pt 9 t '512-KB Aligned'

unset yrange
unset xrange
unset grid
unset key

# Matrix. ----------------------------------------------------------------------
set output plot_file_1

xtics_min = 64
xtics_max = 3072
xtics_inc = 64

xrange_min = xtics_min - (xtics_inc / 2)
xrange_max = xtics_max + (xtics_inc / 2)

ytics_min = 0
ytics_max = 14

yrange_min = ytics_min - 0.5
yrange_max = ytics_max + 0.5

cbtics_min = 0
cbtics_max = 1
cbtics_inc = 0.1

set xrange[xrange_min:xrange_max]
set xtics xtics_min, xtics_inc, xtics_max font ',10' rotate
set xlabel '| PTE Set |' font ',12' offset 0,1.5

set yrange[yrange_min:yrange_max]
set ytics ('4-KB'   0, '8-KB'   1, '16-KB'   2, '32-KB'   3, '64-KB'   4, \
           '128-KB' 5, '256-KB' 6, '512-KB'  7, '1-MB'    8, '2-MB'    9, \
           '4-MB'  10, '8-MB'  11, '16-MB'  12, '32-MB'  13, '64-MB'  14) font ',10'
set ylabel 'PTE Set Alignment' font ',12' offset 2,0

set cbtics cbtics_min, cbtics_inc, cbtics_max font ',10'
set cblabel 'Eviction Rate' font ',12' offset 0,0

set palette rgb 30,31,32

plot data_file u (($2 + 1) * xtics_inc):($1):($3) matrix with image
