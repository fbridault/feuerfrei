#!/bin/bash

# Retaille et converti en eps
# L'argument de retaillage est la forme widthxheight{+-}x{+-}y{%} 
CROPOPTS="$1"

for IMAGE in $*; do
	if [ -f $IMAGE ]; then
		NEWFILE="${IMAGE/png/eps}"
		convert -crop $CROPOPTS $IMAGE $NEWFILE
	fi
done
