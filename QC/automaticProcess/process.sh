#!/bin/bash
#echo Processing $1 $2 $3

# check calib
if [ $1 == "cpass0" ]; then
  if [ -d $PATH_CALIB/$CURRENT_YEAR/$3 ]; then
#    echo "calibrations does exist."
echo $1 $2 $3 >/tmp/last
  else
    echo "starting calibrations $1 $2 $3"
    cd $PATH_CALIB/$CURRENT_YEAR/
    ./runchain.sh $2 $3
    cd -
  fi
fi

# check QC
if [ -d $PATH_QC/$CURRENT_YEAR/$1/Run$3 ]; then
#  echo "QC does exist."
echo $1 $2 $3 >/tmp/last
else
  echo "starting QC $1 $2 $3"
  cd $PATH_QC/$CURRENT_YEAR/$1/
  ./getAQCplots.sh $3 $2 $1 $CURRENT_YEAR
  cd -
fi


