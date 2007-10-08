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
	
	moyenne=0
	i=0
	if len(lines) > 1000:
		for line in lines[0:1000]:
			if float(line) < 1.0:
				moyenne = (moyenne*i + float(line))/(i+1)
				#moyenne = moyenne + float(line)
				i = i + 1
			else:
				print "Valeur supprimée : %f" % float(line)
		print "La moyenne est de : %f" % moyenne
	else:
		print "Pas assez de valeurs !"
	
