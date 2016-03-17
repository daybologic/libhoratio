#!/bin/sh -x

set -e

for source in lib/*.[ch]; do
	astyle \
		--indent-switches \
		--indent=force-tab=8 \
		--style=attach \
		--align-pointer=name \
		--add-brackets \
		--max-code-length=78 \
		--keep-one-line-statements \
		--break-blocks=all \
		--indent-classes \
		--lineend=linux \
		--unpad-paren \
		--pad-header \
		"$source";
done

exit 0
