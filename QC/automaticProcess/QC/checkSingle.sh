#!/bin/bash
export nfiles=$(ls $1|wc -l)
if [ $nfiles -lt 24 ]; then
echo rm -rf $1 $nfiles
fi
