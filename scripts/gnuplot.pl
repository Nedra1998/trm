# ===== General configuration =====
# Configure input data format
data = system("echo $DATA")
scene = system("echo $SCENE")
set datafile separator ','
set key autotitle columnhead

# Configure general output format
set terminal pngcairo size 1280,720 enhanced
set grid

# Data pre-processing
getcolumnvalues(x) = sprintf('python -c "data=set([\"-\".join(x.split(\",\")[:%d]) for x in open(\"${DATA}\",\"r\")][1:]);print(*sorted(data))"',x)
commit_seeds = system(getcolumnvalues(2))

print commit_seeds

# ===== Plotting =====
# Samples vs. Time
set title 'Samples vs. Time'
set output scene . '_samples_vs_time.png'
set xlabel 'Samples'
set ylabel 'Time (ms)'

plot for [z=1:words(commit_seeds)] data using 4:((strcol(1) . "-" . strcol(2)) eq word(commit_seeds ,z) ? $5 : 1/0) with linespoints title word(commit_seeds,z)

# Samples vs. Error
set title 'Samples vs. Error'
set output scene . '_samples_vs_error.png'
set xlabel 'Samples'
set ylabel 'Error'

plot for [z=1:words(commit_seeds)] data using 4:((strcol(1) . "-" . strcol(2)) eq word(commit_seeds ,z) ? $6 : 1/0) with linespoints title word(commit_seeds,z)

# Samples vs. Absolute Error
set title 'Samples vs. Absolute Error'
set output scene . '_samples_vs_absolute_error.png'
set xlabel 'Samples'
set ylabel 'Absolute Error'

plot for [z=1:words(commit_seeds)] data using 4:((strcol(1) . "-" . strcol(2)) eq word(commit_seeds ,z) ? $7 : 1/0) with linespoints title word(commit_seeds,z)

# Samples vs. Squared Error
set title 'Samples vs. Squared Error'
set output scene . '_samples_vs_squared_error.png'
set xlabel 'Samples'
set ylabel 'Squared Error'

plot for [z=1:words(commit_seeds)] data using 4:((strcol(1) . "-" . strcol(2)) eq word(commit_seeds ,z) ? $8 : 1/0) with linespoints title word(commit_seeds,z)

# Samples vs. Relative Absolute Error
set title 'Samples vs. Relative Absolute Error'
set output scene . '_samples_vs_relative_absolute_error.png'
set xlabel 'Samples'
set ylabel 'Relative Absolute Error'

plot for [z=1:words(commit_seeds)] data using 4:((strcol(1) . "-" . strcol(2)) eq word(commit_seeds ,z) ? $9 : 1/0) with linespoints title word(commit_seeds,z)

# Samples vs. Squared Error
set title 'Samples vs. Relative Squared Error'
set output scene . '_samples_vs_relative_squared_error.png'
set xlabel 'Samples'
set ylabel 'Relative Squared Error'

plot for [z=1:words(commit_seeds)] data using 4:((strcol(1) . "-" . strcol(2)) eq word(commit_seeds ,z) ? $10 : 1/0) with linespoints title word(commit_seeds,z)

# Samples vs. Delta E
set title 'Samples vs. {/Symbol D}E^*_{ab}'
set output scene . '_samples_vs_delta_e.png'
set xlabel 'Samples'
set ylabel '{/Symbol D}E^*_{ab}'

plot for [z=1:words(commit_seeds)] data using 4:((strcol(1) . "-" . strcol(2)) eq word(commit_seeds ,z) ? $11 : 1/0) with linespoints title word(commit_seeds,z)

# vim:ft=gnuplot
