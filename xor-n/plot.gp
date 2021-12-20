if(!exists('n')) n = 4
if(!exists('data_file')) data_file = 'xor.dat'

if (n > 4) {
  set terminal png font 'Helvetica, 19' size 3840,2160 enhanced
} else {
  set terminal png font 'Helvetica, 19' size 1920,1080 enhanced
}

set output 'xor_'.n.'.png'

s = n * 2
o = 2 ** n

xtics_min = 0
xtics_max = o * o
xtics_inc = o

xrange_min = xtics_min
xrange_max = xtics_max - 1

set xrange[xrange_min:xrange_max]
set xtics xtics_min, xtics_inc, xtics_max font ',10' rotate
set xlabel '\{hi, lo\} ('.s.' bits)' font ',14' offset 0,1

unset key
unset grid

set multiplot layout 2,1

## i. --------------------------------------------------------------------------

ytics_min = 0
ytics_max = o - 1
ytics_inc = 1

yrange_min = ytics_min - 1
yrange_max = ytics_max + 1

set yrange[yrange_min:yrange_max]
set ytics ytics_min, ytics_inc, ytics_max font ',10'
set ylabel 'i ('.n.' bits)' font ',14' offset 1,0

set title 'i = xor\_'.n.'(x) = (lo + '.o.' + t) mod '.o
plot data_file u 1 with lines lc 'black'

## t. --------------------------------------------------------------------------

ytics_min = -(o - 1)
ytics_max = +(o - 1)
ytics_inc = 2

yrange_min = ytics_min - 1
yrange_max = ytics_max + 1

set yrange[yrange_min:yrange_max]
set ytics ytics_min, ytics_inc, ytics_max font ',10'
set ylabel 't' font ',14' offset 1,0

unset title
plot 'xor.dat' u 2 with lines lc 'black'

unset multiplot
