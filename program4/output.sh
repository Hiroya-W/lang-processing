#!/bin/bash
mkdir -p outputs
for filepath in `\find samples/ -maxdepth 2 -type f -name "*.mpl" | sort`; do
    filename="${filepath##*/}"
    ./test $filepath
done
