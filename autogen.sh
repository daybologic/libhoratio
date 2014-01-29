#!/bin/sh

set -e

if [ -f configure ]; then
	echo 'Skipping autoreconf -ivf; configure script already exists'
else
	autoreconf -ivf
fi

exit 0;
