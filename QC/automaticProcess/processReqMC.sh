#!/bin/bash
#echo Processing $1 $2 $3 $4
#                path run period year

# check QC
if [ -d $PATH_QC/sim/$4/Run$2 ]; then
#  echo "QC does exist."
echo $1 $2 $3 >/tmp/last
else
  echo "starting QC $1 $2 $3 $4"
  cd $PATH_QC/sim/$4/
  pwd
  ./getAQCplotsMC.sh $3 $2 $1
  cd -
fi


