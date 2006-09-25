set encoding iso_8859_1

#set terminal x11 font "Helvetica,20"
set terminal png small enhanced

#set time
set autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label
#unset key

set logscale y
set border 4095
set ytic auto                        # set ytics automatically
set xtic auto                        

#set hidden3d

#set title "Erreur RMS commise lors du Gauss-Seidel"
set xlabel "Resolution step"
set ylabel "Residual"

set output 'wGCSSORDiff.u.png'
plot "1.5/residualsAverage.log" using 1:5 title "pouetuetu" with lines, "1.815/residualsAverage.log" using 1:5 with lines
set output 'wGCSSORDiff.v.png'
plot "1.5/residualsAverage.log" using 1:6 with lines, "1.815/residualsAverage.log" using 1:6 with lines
set output 'wGCSSORDiff.w.png'
plot "1.5/residualsAverage.log" using 1:7 with lines, "1.815/residualsAverage.log" using 1:7 with lines
set output 'wGCSSORProj1.png'
plot "1.5/residualsAverage.log" using 1:10 with lines, "1.815/residualsAverage.log" using 1:10 with lines
set output 'wGCSSORProj2.png'
plot "1.5/residualsAverage.log" using 1:11 with lines, "1.815/residualsAverage.log" using 1:11 with lines
