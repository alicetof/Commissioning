#!/bin/sh
export CURRENT_YEAR=${CURRENT_YEAR:-2024}
timeout 60 alien.py cp alien:///alice/data/$CURRENT_YEAR/$3/$1/$5/$4/$2/o2calib_tof.root file:$2.root &

export status=$?
if [ $status -gt 0 ]; then
rm $2.root
fi

