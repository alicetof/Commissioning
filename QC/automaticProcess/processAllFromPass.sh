#!/bin/bash
rm $PATH_QC/$CURRENT_YEAR/$1/periods
./getRunReady.sh $1
cat listaDone_$1 |awk '{print "./process.sh '$1'",$1,$2}'|bash

# finalize QC
cd $PATH_QC/$CURRENT_YEAR/$1
./mkreports.sh $1
cd -
