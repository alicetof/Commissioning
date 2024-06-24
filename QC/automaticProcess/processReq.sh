#!/bin/bash
#echo Processing $1 $2 $3 $4
#                pass period run year 

# check QC
if [ -d $PATH_QC/$4/$1/Run$3 ]; then
#  echo "QC does exist."
echo $1 $2 $3 >/tmp/last
else
  echo "starting QC $1 $2 $3"
  cd $PATH_QC/$4/$1/
  ./getAQCplots.sh $3 $2 $1 $4
  cd -
fi


