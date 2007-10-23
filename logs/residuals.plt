set encoding iso_8859_1

#set terminal x11 font "Helvetica,20"
set terminal png small enhanced
#set terminal postscript eps enhanced color 22 lw 2

set view 50,230

#set time
set autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label
unset key

set border 4095
set logscale z
set ytic auto                        # set ytics automatically
set xtic auto                        
set ztic auto

set pm3d at s
set log cb
#set cbrange [1e-06:1e-21]
set colorbox horizontal
set colorbox user origin 0.1,0.07 size 0.8,0.02

set surface
#set hidden3d

#set title "Erreur RMS commise lors du Gauss-Seidel"
set xlabel "Pas de temps" -2,0,0
set ylabel "Numéro de l'itération" 4,0,0
set zlabel "Résidu" 2,4,0

set output 'GSsolverDiff.u.eps'
splot "GSsolverDiff.u.log" using 1:2:3 with pm3d
set output 'GSsolverDiff.v.eps'
splot "GSsolverDiff.v.log" using 1:2:3 with pm3d
set output 'GSsolverDiff.w.eps'
splot "GSsolverDiff.w.log" using 1:2:3 with pm3d
set output 'GCSSORsolverDiff.u.eps'
splot "GCSSORsolverDiff.u.log" using 1:2:3 with pm3d
set output 'GCSSORsolverDiff.v.eps'
splot "GCSSORsolverDiff.v.log" using 1:2:3 with pm3d
set output 'GCSSORsolverDiff.w.eps'
splot "GCSSORsolverDiff.w.log" using 1:2:3 with pm3d
set output 'GSsolverProj1.eps'
splot "GSsolverProj1.log" using 1:2:3 with pm3d
set output 'GSsolverProj2.eps'
splot "GSsolverProj2.log" using 1:2:3 with pm3d
set output 'GCSSORsolverProj1.eps'
splot "GCSSORsolverProj1.log" using 1:2:3 with pm3d
set output 'GCSSORsolverProj2.eps'
splot "GCSSORsolverProj2.log" using 1:2:3 with pm3d
