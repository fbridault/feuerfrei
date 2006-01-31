set encoding iso_8859_1

#set terminal x11 font "Helvetica,20"
set terminal png small enhanced

#set time
set autoscale                        # scale axes automatically

set logscale y
set border 4095
set ytic auto                        # set ytics automatically
set xtic auto                        

#set hidden3d

#set title "Erreur RMS commise lors du Gauss-Seidel"
set xlabel "Time"
set ylabel "Residual"

set output 'GC-GCSSORDiffTimeRes.u.png'
plot "residualsAverage.log" using 2:3 with lines title "GS", "residualsAverage.log" using 8:9 with lines title "CGSSOR"
set output 'GC-GCSSORDiffTimeRes.v.png'
plot "residualsAverage.log" using 4:5 with lines title "GS", "residualsAverage.log" using 10:11 with lines title "CGSSOR"
set output 'GC-GCSSORDiffTimeRes.w.png'
plot "residualsAverage.log" using 6:7 with lines title "GS", "residualsAverage.log" using 12:13 with lines title "CGSSOR"
set output 'GC-GCSSORProjTimeRes1.png'
plot "residualsAverage.log" using 14:15 with lines title "GS", "residualsAverage.log" using 18:19 with lines title "CGSSOR"
set output 'GC-GCSSORProjTimeRes2.png'
plot "residualsAverage.log" using 16:17 with lines title "GS", "residualsAverage.log" using 20:21 with lines title "CGSSOR"

unset log
set xlabel "Resolution step"
set ylabel "Time"

set output 'GC-GCSSORDiffTime.u.png'
plot "residualsAverage.log" using 1:2 with lines title "GS", "residualsAverage.log" using 1:8 with lines title "CGSSOR"
set output 'GC-GCSSORDiffTime.v.png'
plot "residualsAverage.log" using 1:4 with lines title "GS", "residualsAverage.log" using 1:10 with lines title "CGSSOR"
set output 'GC-GCSSORDiffTime.w.png'
plot "residualsAverage.log" using 1:6 with lines title "GS", "residualsAverage.log" using 1:12 with lines title "CGSSOR"
set output 'GC-GCSSORProjTime1.png'
plot "residualsAverage.log" using 1:14 with lines title "GS", "residualsAverage.log" using 1:18 with lines title "CGSSOR"
set output 'GC-GCSSORProjTime2.png'
plot "residualsAverage.log" using 1:16 with lines title "GS", "residualsAverage.log" using 1:20 with lines title "CGSSOR"
