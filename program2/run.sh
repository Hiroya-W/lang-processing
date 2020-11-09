#!/bin/bash
for file in `\find ./samples/ -maxdepth 1 -type f | sort`; do
    echo $file >> output.txt
    ./main $file  >> output.txt
    echo -e '\n' >> output.txt
done
