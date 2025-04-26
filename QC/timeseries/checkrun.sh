#!/bin/bash
export period=$2
export year=$(echo $2|awk -F"LHC" '{print "20"substr($2,1,2)}')

echo $year $period $1
alien.py find  /alice/data/$year/${period}/$1/apass5/ time_series_tracks_0.root |grep Stage_5
