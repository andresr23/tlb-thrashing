set terminal png font 'Helvetica, 19' size 960,1080 enhanced
if(!exists('data_file_0')) data_file_0 = 'rate-skylake.dat'
if(!exists('data_file_1')) data_file_1 = 'sets-skylake.dat'
if(!exists('plot_file')) plot_file = 'plot.png'
set output plot_file

xtics_min = 16
xtics_max = 128
xtics_inc = 16

xrange_min = xtics_min - (xtics_inc / 2)
xrange_max = xtics_max + (xtics_inc / 2)

set xrange[xrange_min:xrange_max]
set xtics xtics_min, xtics_inc, xtics_max font ',10'
set xlabel '| PTE Set | (L1-dTLB Flooding)' font ',12'

set grid
set key left top box font ',11'
unset title

set multiplot layout 2,1

# Rate. ------------------------------------------------------------------------

ytics_min = 0
ytics_max = 1
ytics_inc = 0.1

yrange_min = ytics_min - ytics_inc
yrange_max = ytics_max + ytics_inc

set yrange[yrange_min:yrange_max]
set ytics ytics_min, ytics_inc, ytics_max font ',10'
set ylabel 'Flooding Rate' font ',12' offset 2,0

plot data_file_0 u (($0 + 1) * xtics_inc):($1) with linespoints lc 'red'    lw 1.5 lt 1 pt 7  t 'pass = 1', \
     data_file_0 u (($0 + 1) * xtics_inc):($2) with linespoints lc 'blue'   lw 1.5 lt 0 pt 9  t 'pass = 2', \
     data_file_0 u (($0 + 1) * xtics_inc):($3) with linespoints lc 'green'  lw 1.5 lt 1 pt 11 t 'pass = 3', \
     data_file_0 u (($0 + 1) * xtics_inc):($4) with linespoints lc 'purple' lw 1.5 lt 0 pt 13 t 'pass = 4'

# Sets. ------------------------------------------------------------------------

ytics_min = 0
ytics_max = 16
ytics_inc = 1

yrange_min = ytics_min - ytics_inc
yrange_max = ytics_max + ytics_inc

set yrange[yrange_min:yrange_max]
set ytics ytics_min, ytics_inc, ytics_max font ',10'
set ylabel 'Evicted PTEs' font ',12' offset 2,0

plot data_file_1 u (($0 + 1) * xtics_inc):($1) with linespoints lc 'red'    lw 1.5 lt 1 pt 7  t 'pass = 1', \
     data_file_1 u (($0 + 1) * xtics_inc):($2) with linespoints lc 'blue'   lw 1.5 lt 0 pt 9  t 'pass = 2', \
     data_file_1 u (($0 + 1) * xtics_inc):($3) with linespoints lc 'green'  lw 1.5 lt 1 pt 11 t 'pass = 3', \
     data_file_1 u (($0 + 1) * xtics_inc):($4) with linespoints lc 'purple' lw 1.5 lt 0 pt 13 t 'pass = 4'

unset multiplot
