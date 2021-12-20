set terminal png font 'Helvetica, 19' size 1080,540 enhanced
if(!exists('data_file')) data_file = 'rate-skylake.dat'
if(!exists('plot_file')) plot_file = 'plot.png'
set output plot_file

xtics_min = 1
xtics_max = 32
xtics_inc = 1

xrange_min = xtics_min - 0.5
xrange_max = xtics_max + 0.5

ytics_min = 0
ytics_max = 1
ytics_inc = 0.1

yrange_min = ytics_min - 0.1
yrange_max = ytics_max + 0.1

set xrange[xrange_min:xrange_max]
set xtics xtics_min, xtics_inc, xtics_max font ',10'
set xlabel '| PTE Set | (L2-sTLB Set Eviction)' font ',12'

set yrange[yrange_min:yrange_max]
set ytics ytics_min, ytics_inc, ytics_max font ',10'
set ylabel 'Eviction Rate' font ',12' offset 2,0

set grid

unset key
unset title

plot data_file u ($0 + 1):($1) with linespoints lc 'red' lw 1.5 lt 1 pt 7
