#!/bin/bash
for filepath in `\find ../samples/ -maxdepth 2 -type f | sort`; do
    filename="${filepath##*/}"
    # ./main $filepath  > ./outputs/$filename
    ./main $filepath
done
