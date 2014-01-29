#!/bin/sh
# This script is designed for in-tree maintainers, and is not used by the
# Debian package.

make -f debian/rules maintainer
exit $?;
