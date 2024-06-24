#!/bin/bash
export period=$(./getPeriod.sh $1)
export run=$1

./upload.sh $period $run
./upload2.sh $period $run

