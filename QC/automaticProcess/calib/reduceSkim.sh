#!/bin/bash

cd $1

ls skim*root >fulllist

cp ../reduce.C .
root -b -q -l reduce.C

rm skim*root
cd ..
