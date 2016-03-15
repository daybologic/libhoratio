#!/bin/sh -x

set -e

for source in lib/*.[ch]; do
	astyle --indent=force-tab=8 --style=attach "$source";
done

exit 0
