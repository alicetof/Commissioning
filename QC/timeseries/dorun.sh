#!/bin/bash
export period=$2
export year=$(echo $2|awk -F"LHC" '{print "20"substr($2,1,2)}')

echo alien.py find  /alice/data/$year/${period}/$1/apass5/ time_series_tracks_0.root 

alien.py find  /alice/data/$year/${period}/$1/apass5/ time_series_tracks_0.root |grep Stage_5 >lista
rm -rf $1
mkdir $1
cat -b lista|awk '{print "./filter.sh","alien://"$2,"'$1'/"$1".root"}'|bash

ls $1/*.root >listaM ; root -b -q -l DoMerge.C\(\"listaM\",\"$1.root\"\)
