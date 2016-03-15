#!/bin/sh -x

set -e

for source in lib/*.[ch]; do
	astyle --indent-switches --indent=force-tab=8 --style=attach "$source";
done

exit 0
