#!/bin/bash


du -hs accumulated/$1* -B 1|awk '{sum+=$1;} END{print sum*1E-9;}'

ls $1|grep skim_|wc -l|xargs echo Nfiles=

