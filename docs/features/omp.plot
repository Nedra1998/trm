# Data
set datafile separator ","

# Themeing
set border linewidth 2 linecolor rgb "#4c4f69"
set style line 10 lt 10 lc rgb "#9ca0b0" lw 1 dt 2
set style line 1 lc rgb "#1e66f5"
set style line 2 lc rgb "#d20f39"
set style line 3 lc rgb "#40a02b"

# Histogram
set style data histogram
set style histogram errorbars gap 1 lw 1
set style fill solid border lc rgb "#4c4f69"
set boxwidth 0.9
set object 1 rectangle from graph 0, graph 0 to graph 1, graph 1 behind fc rgb '#dce0e8' fs noborder

# Grid           
set ytics
set mytics
set grid y ls 10

# Title
set title "Time Per Sample vs. Chunk Size" textcolor rgb "#4c4f69"
set xlabel "Sample Count" textcolor rgb "#4c4f69"
set ylabel "Time Per Sample (ms)" textcolor rgb "#4c4f69"
set key inside top right

# Output
set terminal pngcairo size 1280,720 enhanced font "Iosevka Nerd Font,12" transparent background "#eff1f5"
set output 'omp.png'


# Plot
plot 'omp.csv' using 2:3:xtic(1) title columnheader(2) ls 1, \
     'omp.csv' using 4:5:xtic(1) title columnheader(4) ls 2, \
     'omp.csv' using 6:7:xtic(1) title columnheader(6) ls 3
# vim:ft=gnuplot
