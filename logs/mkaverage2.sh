#!/bin/bash

# Script servant à générer un graphique contenant toutes les moyennes des résidus
# pour différentes valeurs de omega.

# On définit une commande pour les u,v,w de la diffusion et les deux projections.
PLOT_CMD1="plot "
PLOT_CMD2="plot "
PLOT_CMD3="plot "
PLOT_CMD4="plot "
PLOT_CMD5="plot "

for FILE in `ls -l`; do
	if [ -d $FILE ]; then
		PLOT_CMD1="${PLOT_CMD1} \"$FILE/residualsAverage.log\" using 1:5 title \"$FILE\" with lines,"
		PLOT_CMD2="${PLOT_CMD2} \"$FILE/residualsAverage.log\" using 1:6 title \"$FILE\" with lines,"
		PLOT_CMD3="${PLOT_CMD3} \"$FILE/residualsAverage.log\" using 1:7 title \"$FILE\" with lines,"
		PLOT_CMD4="${PLOT_CMD4} \"$FILE/residualsAverage.log\" using 1:10 title \"$FILE\" with lines,"
		PLOT_CMD5="${PLOT_CMD5} \"$FILE/residualsAverage.log\" using 1:11 title \"$FILE\" with lines,"
	fi
done

# On supprime la dernière virgule inutile
PLOT_CMD1=${PLOT_CMD1/%,}
PLOT_CMD2=${PLOT_CMD2/%,}
PLOT_CMD3=${PLOT_CMD3/%,}
PLOT_CMD4=${PLOT_CMD4/%,}
PLOT_CMD5=${PLOT_CMD5/%,}

gnuplot << EOF
set encoding iso_8859_1

#set terminal x11 font "Helvetica,20"
#set terminal png small enhanced
set terminal postscript eps enhanced color 22 lw 2

#set time
set autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label

set logscale y
set border 4095
set ytic auto                        # set ytics automatically
set xtic auto                        

#set hidden3d

#set title "Erreur RMS commise lors du Gauss-Seidel"
set xlabel "Itération"
set ylabel "Résidu"


set style increment user
set style line 10 lt 2 lc rgb "orange-red"
set style line 11 lt 3 lc rgb "light-pink"
set style line 12 lt 4 lc rgb "midnight-blue"
set style line 13 lt 5 lc rgb "dark-goldenrod"
set style line 14 lt 6 lc rgb "khaki"
set style line 15 lt 3 lc rgb "turquoise"
set style line 16 lt 8 lc rgb "plum"
set style line 17 lt 1 lc rgb "gold"
set style line 18 lt 0 lc rgb "dark-violet"
set style line 19 lt 1 lc rgb "forest-green"

set output 'wGCSSORDiff.u.eps'
$PLOT_CMD1
set output 'wGCSSORDiff.v.eps'
$PLOT_CMD2
set output 'wGCSSORDiff.w.eps'
$PLOT_CMD3
set key left bottom
set output 'wGCSSORProj1.eps'
$PLOT_CMD4
set output 'wGCSSORProj2.eps'
$PLOT_CMD5
EOF
