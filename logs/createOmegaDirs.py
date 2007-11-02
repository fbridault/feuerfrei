#!/usr/bin/python

import os,sys

for i in range(1,20):
	path="%.1f" % (i/10.0)
	os.mkdir (path)
