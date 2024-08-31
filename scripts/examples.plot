# ===== General configuration =====
# Configure input data format
data = system("echo $DATA")
set datafile separator ','
set key autotitle columnhead

# Themeing
set border linewidth 2 linecolor rgb "#4c4f69"
set style line 999 lt 999 lc rgb "#9ca0b0" lw 1 dt 2
set object 1 rectangle from graph 0, graph 0 to graph 1, graph 1 behind fc rgb '#dce0e8' fs noborder
set style line 1 lc rgb "#1e66f5"
set style line 2 lc rgb "#d20f39"
set style line 3 lc rgb "#40a02b"
set style line 4 lc rgb "#8839ef"
set style line 5 lc rgb "#fe640b"


# Configure general output format
set terminal pngcairo size 1050,567 enhanced font "Iosevka Nerd Font,12" transparent background "#eff1f5"
set grid ls 999

# Data pre-processing
getcolumn(f,x) = sprintf('python -c "data=set([x.split(\",\")[%d] for x in open(\"%s\",\"r\")][1:]);print(*sorted(data))"',x, f)
getcolumns(f) = sprintf('python -c "print(*sorted([f\"{c}-{s}\" for c, s in {line.split(\",\")[0]: line.split(\",\")[1] for line in open(\"%s\")}.items()]))"', f)

# ===== Plotting =====
# ----- Box -----
data = system("echo $DATA") . '/box.csv'
scene = 'Box'
lscene = 'box'
commit_seeds = system(getcolumns(data))
commits = system(getcolumn(data, 0))
# Samples vs. Time
set title scene . ': Samples vs. Time' textcolor rgb "#4c4f69"
set xlabel 'Samples' textcolor rgb "#4c4f69"
set ylabel 'Time (s)' textcolor rgb "#4c4f69"
set output './docs/examples/' . lscene . '_samples_vs_time.png' 

plot for [z=1:words(commit_seeds)] data using 4:((strcol(1) . "-" . strcol(2)) eq word(commit_seeds ,z) ? $5 / 1000 : 1/0) with linespoints linestyle z title word(commits,z)

# Samples vs. Delta E
set title scene . ': Samples vs. {/Symbol D}E' textcolor rgb "#4c4f69"
set xlabel 'Samples' textcolor rgb "#4c4f69"
set ylabel '{/Symbol D}E' textcolor rgb "#4c4f69"
set output './docs/examples/' . lscene . '_samples_vs_delta_e.png'

plot for [z=1:words(commit_seeds)] data using 4:((strcol(1) . "-" . strcol(2)) eq word(commit_seeds ,z) ? $11 : 1/0) with linespoints linestyle z title word(commits,z)

# ----- Box -----
data = system("echo $DATA") . '/basic.csv'
scene = 'Basic'
lscene = 'basic'
commit_seeds = system(getcolumns(data))
commits = system(getcolumn(data, 0))
# Samples vs. Time
set title scene . ': Samples vs. Time' textcolor rgb "#4c4f69"
set xlabel 'Samples' textcolor rgb "#4c4f69"
set ylabel 'Time (s)' textcolor rgb "#4c4f69"
set output './docs/examples/' . lscene . '_samples_vs_time.png' 

plot for [z=1:words(commit_seeds)] data using 4:((strcol(1) . "-" . strcol(2)) eq word(commit_seeds ,z) ? $5 / 1000 : 1/0) with linespoints linestyle z title word(commits,z)

# Samples vs. Delta E
set title scene . ': Samples vs. {/Symbol D}E' textcolor rgb "#4c4f69"
set xlabel 'Samples' textcolor rgb "#4c4f69"
set ylabel '{/Symbol D}E' textcolor rgb "#4c4f69"
set output './docs/examples/' . lscene . '_samples_vs_delta_e.png'

plot for [z=1:words(commit_seeds)] data using 4:((strcol(1) . "-" . strcol(2)) eq word(commit_seeds ,z) ? $11 : 1/0) with linespoints linestyle z title word(commits,z)

# ----- Spheres -----
data = system("echo $DATA") . '/spheres.csv'
scene = 'Spheres'
lscene = 'spheres'
commit_seeds = system(getcolumns(data))
commits = system(getcolumn(data, 0))
# Samples vs. Time
set title scene . ': Samples vs. Time' textcolor rgb "#4c4f69"
set xlabel 'Samples' textcolor rgb "#4c4f69"
set ylabel 'Time (s)' textcolor rgb "#4c4f69"
set output './docs/examples/' . lscene . '_samples_vs_time.png' 

plot for [z=1:words(commit_seeds)] data using 4:((strcol(1) . "-" . strcol(2)) eq word(commit_seeds ,z) ? $5 / 1000 : 1/0) with linespoints linestyle z title word(commits,z)

# Samples vs. Delta E
set title scene . ': Samples vs. {/Symbol D}E' textcolor rgb "#4c4f69"
set xlabel 'Samples' textcolor rgb "#4c4f69"
set ylabel '{/Symbol D}E' textcolor rgb "#4c4f69"
set output './docs/examples/' . lscene . '_samples_vs_delta_e.png'

plot for [z=1:words(commit_seeds)] data using 4:((strcol(1) . "-" . strcol(2)) eq word(commit_seeds ,z) ? $11 : 1/0) with linespoints linestyle z title word(commits,z)

# ----- Menger -----
data = system("echo $DATA") . '/menger.csv'
scene = 'Menger'
lscene = 'menger'
commit_seeds = system(getcolumns(data))
commits = system(getcolumn(data, 0))
# Samples vs. Time
set title scene . ': Samples vs. Time' textcolor rgb "#4c4f69"
set xlabel 'Samples' textcolor rgb "#4c4f69"
set ylabel 'Time (s)' textcolor rgb "#4c4f69"
set output './docs/examples/' . lscene . '_samples_vs_time.png' 

plot for [z=1:words(commit_seeds)] data using 4:((strcol(1) . "-" . strcol(2)) eq word(commit_seeds ,z) ? $5 / 1000 : 1/0) with linespoints linestyle z title word(commits,z)

# Samples vs. Delta E
set title scene . ': Samples vs. {/Symbol D}E' textcolor rgb "#4c4f69"
set xlabel 'Samples' textcolor rgb "#4c4f69"
set ylabel '{/Symbol D}E' textcolor rgb "#4c4f69"
set output './docs/examples/' . lscene . '_samples_vs_delta_e.png'

plot for [z=1:words(commit_seeds)] data using 4:((strcol(1) . "-" . strcol(2)) eq word(commit_seeds ,z) ? $11 : 1/0) with linespoints linestyle z title word(commits,z)

# vim:ft=gnuplot
