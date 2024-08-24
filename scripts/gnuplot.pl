# ===== General configuration =====
# Configure input data format
dir = system("echo $DATA_DIR")
scene = system("echo $SCENE")
set datafile separator ','
set key autotitle columnhead

# Configure general output format
set terminal pngcairo size 1280,720
set grid

# ===== Plotting =====
# Samples vs. Time
set title 'Samples vs. Time'
set output scene . '_samples_vs_time.png'
set xlabel 'Samples'
set ylabel 'Time (ms)'

plot for [i=2:*] dir . '/' . scene . '_time.csv' using 1:i with linespoints

# Samples vs. Error
set title 'Samples vs. Error'
set output scene . '_samples_vs_error.png'
set xlabel 'Samples'
set ylabel 'Error'

plot for [i=2:*] dir . '/' . scene . '_error.csv' using 1:i with linespoints

# Samples vs. Absolute Error
set title 'Samples vs. Absolute Error'
set output scene . '_samples_vs_absolute_error.png'
set xlabel 'Samples'
set ylabel 'Absolute Error'

plot for [i=2:*] dir . '/' . scene . '_absoluteError.csv' using 1:i with linespoints

# Samples vs. Squared Error
set title 'Samples vs. Squared Error'
set output scene . '_samples_vs_squared_error.png'
set xlabel 'Samples'
set ylabel 'Squared Error'

plot for [i=2:*] dir . '/' . scene . '_squaredError.csv' using 1:i with linespoints

# Samples vs. Relative Absolute Error
set title 'Samples vs. Relative Absolute Error'
set output scene . '_samples_vs_relative_absolute_error.png'
set xlabel 'Samples'
set ylabel 'Relative Absolute Error'

plot for [i=2:*] dir . '/' . scene . '_relativeAbsoluteError.csv' using 1:i with linespoints

# Samples vs. Squared Error
set title 'Samples vs. Relative Squared Error'
set output scene . '_samples_vs_relative_squared_error.png'
set xlabel 'Samples'
set ylabel 'Relative Squared Error'

plot for [i=2:*] dir . '/' . scene . '_relativeSquaredError.csv' using 1:i with linespoints

# vim:ft=gnuplot
