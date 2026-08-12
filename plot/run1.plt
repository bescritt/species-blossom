set multiplot
set xlabel "scanner"
set ylabel "paper"
set zlabel "stone"
splot	'run1.txt' index 0 title 'Coreclears, Oneshots and Twoshot',\
	'run1.txt' index 1 title 'Clear-directing Scanner',\
	'run1.txt' index 2 title 'Paper',\
	'run1.txt' index 3 title 'Paper with 3/7-point imps',\
	'run1.txt' index 4 title 'Paper with seperate stone',\
	'run1.txt' index 5 title 'Stones with a-imps',\
	'run1.txt' index 6 title 'Stones with b-imps',\
	'run1.txt' index 7 title 'Scanner',\
	'run1.txt' index 8 title 'Bomber and Vampires without imps'
unset multiplot
pause -1
set terminal png
set output 'run1.png'
replot
