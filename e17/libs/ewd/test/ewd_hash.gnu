
set term x11
# set sample 20000

#
#  Comparison graph
#

set xlabel 'Array position'
set ylabel 'Keys in position'
set title \
"Frequency of key placement in the hash table"

plot 'ewd_hash.dat' u 1:2 t '# of keys' w p

#
#  Generate postscript of Comparison graph
#

pause -1 'press return to generate postscript file, ewd_hash.ps'

set term postscript
set output 'ewd_hash.ps'
replot

set term dumb # x11
set autoscale
set title ''
set xlabel ''
set ylabel ''
set xrange [-10:10]
set yrange [-10:10]
set key

