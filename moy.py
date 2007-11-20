#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Remplacement du type de field
import re
import sys
import string

for file in sys.argv[1:]:
	print "Processing %s..." % file
	# On stocke le fichier dans une séquence de chaînes
	f=open(file, 'r')
	lines=f.readlines()
	f.close()
	
	#moyenne=0
	#count
	moyenne = [0.0, 0.0, 0.0]
	count = [0.0, 0.0, 0.0]
	if len(lines) > 1000:
		for line in lines[0:1000]:
			values = line.split(' ')
			for j,value in enumerate(values):
				if float(value) < 100000.0:
					moyenne[j] = (moyenne[j]*count[j] + float(value))/(count[j]+1)
					count[j] = count[j] + 1
				else:
					print "Valeur supprimée : %f" % float(value)
		print "Les moyennes sont : "
		for j in moyenne:
			print j*1000
	else:
		print "Pas assez de valeurs !"
