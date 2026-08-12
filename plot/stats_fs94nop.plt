set multiplot
set xlabel "scanner"
set ylabel "paper"
set zlabel "stone"
splot	'stats_fs94nop.dat' index 0 title 'Coreclears, Oneshots and Twoshot',\
	'stats_fs94nop.dat' index 1 title 'Clear-directing Scanner',\
	'stats_fs94nop.dat' index 2 title 'Paper',\
	'stats_fs94nop.dat' index 3 title 'Paper with 3/7-point imps',\
	'stats_fs94nop.dat' index 4 title 'Paper with seperate stone',\
	'stats_fs94nop.dat' index 5 title 'Stones with a-imps',\
	'stats_fs94nop.dat' index 6 title 'Stones with b-imps',\
	'stats_fs94nop.dat' index 7 title 'Scanner',\
	'stats_fs94nop.dat' index 8 title 'Bomber and Vampires without imps'
unset multiplot
pause -1 "press enter to close window, and start save to png file"
set terminal png
set output 'stats_fsh94nop.png'
replot
