set encoding iso_8859_1

#set terminal x11 font "Helvetica,20"
set terminal png small enhanced

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
set xlabel "Simulation time step"
set ylabel "Resolution step"
set zlabel "Residu"

set output 'GSsolverDiff.u.png'
splot "GSsolverDiff.u.log" using 1:2:3 with pm3d
set output 'GSsolverDiff.v.png'
splot "GSsolverDiff.v.log" using 1:2:3 with pm3d
set output 'GSsolverDiff.w.png'
splot "GSsolverDiff.w.log" using 1:2:3 with pm3d
set output 'GCSSORsolverDiff.u.png'
splot "GCSSORsolverDiff.u.log" using 1:2:3 with pm3d
set output 'GCSSORsolverDiff.v.png'
splot "GCSSORsolverDiff.v.log" using 1:2:3 with pm3d
set output 'GCSSORsolverDiff.w.png'
splot "GCSSORsolverDiff.w.log" using 1:2:3 with pm3d
set output 'GSsolverProj1.png'
splot "GSsolverProj1.log" using 1:2:3 with pm3d
set output 'GSsolverProj2.png'
splot "GSsolverProj2.log" using 1:2:3 with pm3d
set output 'GCSSORsolverProj1.png'
splot "GCSSORsolverProj1.log" using 1:2:3 with pm3d
set output 'GCSSORsolverProj2.png'
splot "GCSSORsolverProj2.log" using 1:2:3 with pm3d
