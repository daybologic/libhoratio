#!/bin/sh

set -e

OUTPUT='hversion.h'
LOG='../debian/changelog'

ident=`../utils/hgident`

version=`awk -F'[()]' '{print $2; count++; if (count==2) exit}' $LOG`

echo "#define HORATIO_VERSION_IDENT \"$ident\"" > $OUTPUT
echo "#define HORATIO_VERSION \"$version\"" >> $OUTPUT

set `echo $version | tr "." "\n"`

printf "#define HORATIO_VERSION_MAJOR (%d)\n" "$1" >> $OUTPUT
printf "#define HORATIO_VERSION_MINOR (%d)\n" "$2" >> $OUTPUT
printf "#define HORATIO_VERSION_PATCH (%d)\n" "$3" >> $OUTPUT

exit 0;
