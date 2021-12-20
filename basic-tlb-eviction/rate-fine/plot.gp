set terminal png font 'Helvetica, 19' size 1620,540 enhanced
if(!exists('data_file')) data_file = 'rate-skylake.dat'
if(!exists('plot_file')) plot_file = 'plot-skylake.png'
set output plot_file

xtics_min = 1344
xtics_max = 1664
xtics_inc = 8

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

unset title
set key left top box font ',11'

plot data_file u ($0 * xtics_inc + xtics_min):($1) with linespoints lc 'red'  lw 1 lt 1 pt 7 t 'Non-aligned', \
     data_file u ($0 * xtics_inc + xtics_min):($2) with linespoints lc 'blue' lw 1 lt 0 pt 9 t '512-KB Aligned'
