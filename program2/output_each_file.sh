#!/bin/bash
for filepath in `\find ./samples/ -maxdepth 1 -type f | sort`; do
    filename="${filepath##*/}"
    ./main $filepath  > ./outputs/$filename
done
