set encoding iso_8859_1

#set terminal x11 font "Helvetica,26"
set terminal png small enhanced font arial 14
#set terminal postscript png enhanced color 22 lw 2

#set time
set autoscale                        # scale axes automatically
unset log                              # remove any log-scaling

set logscale y
set border 4095
set ytic auto                        # set ytics automatically
set xtic auto                        

#set hidden3d

#set title "Précision dans l'étape de diffusion"
set xlabel "Itération"
set ylabel "Résidu"

set output 'GC-GCSSORDiff.u.png'
plot "residualsAverage.log" using 1:2 with lines title "GS", "residualsAverage.log" using 1:5 with lines title "CGSSOR"
set output 'GC-GCSSORDiff.v.png'
plot "residualsAverage.log" using 1:3 with lines title "GS", "residualsAverage.log" using 1:6 with lines title "CGSSOR"
set output 'GC-GCSSORDiff.w.png'
plot "residualsAverage.log" using 1:4 with lines title "GS", "residualsAverage.log" using 1:7 with lines title "CGSSOR"

set title "Précision dans l'étape de projection"
set output 'GC-GCSSORProj1.png'
plot "residualsAverage.log" using 1:8 with lines title "GS", "residualsAverage.log" using 1:10 with lines title "CGSSOR"
set output 'GC-GCSSORProj2.png'
plot "residualsAverage.log" using 1:9 with lines title "GS", "residualsAverage.log" using 1:11 with lines title "CGSSOR"
