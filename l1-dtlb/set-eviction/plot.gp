set terminal png font 'Helvetica, 19' size 960,540 enhanced
if(!exists('data_file')) data_file = 'rate-skylake.dat'
if(!exists('plot_file')) plot_file = 'plot.png'
set output plot_file

xtics_min = 1
xtics_max = 10
xtics_inc = 1

xrange_min = xtics_min - 0.5
xrange_max = xtics_max + 0.5

ytics_min = 0
ytics_max = 7

yrange_min = ytics_min - 0.5
yrange_max = ytics_max + 0.5

cbtics_min = 0
cbtics_max = 1
cbtics_inc = 0.2

cbrange_min = cbtics_min
cbrange_max = cbtics_max

set xrange[xrange_min:xrange_max]
set xtics xtics_min, xtics_inc, xtics_max font ',10'
set xlabel '| PTE Set | (L1-dTLB Set Eviction)' font ',12'

set yrange[yrange_min:yrange_max]
set ytics ("4-KB (1)" 0, "8-KB (2)" 1, "16-KB (4)" 2, "32-KB (8)" 3, "64-KB (16)" 4, "128-KB (32)" 5, "256-KB (64)" 6, "512-KB (128)" 7) font ',10'
set ylabel 'Alignment (L1-dTLB Sets)' font ',12' offset 4,0

set cbrange[cbrange_min:cbrange_max]
set cbtics cbtics_min, cbtics_inc, cbtics_max font ',10'
set cblabel 'Eviction Rate' font ',12'

unset key
unset title

set palette defined (0 'cyan', 1 'white')

plot data_file u ($1 + 1):($2):($3) matrix with image, \
     data_file u ($1 + 1):($2):(sprintf('%.3f', $3)) matrix with labels font ',10'
