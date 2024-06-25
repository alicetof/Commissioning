#!/bin/bash

export type=$1
export year=$2
export period=$3
export pass=$4

if [ $type == "data" ]; then

if [ ! -d $PATH_QC/$year/$pass ]; then
  echo "create dir"
  mkdir $PATH_QC/$year
  cp -r $PATH_QC/template $PATH_QC/$year/$pass
fi
rm $PATH_QC/$year/$pass/periods
alien.py find /alice/$type/$year/$period QC_fullrun.root |grep "$pass/"|awk -F"/" '{print $5,$6}' >listaDone_$pass
cat listaDone_$pass |awk '{print "./processReq.sh '$pass'",$1,$2,"'$year'"}'|bash

cd $PATH_QC/$year/$pass
./mkreports.sh $pass
cd -

fi

if [ $type == "sim" ]; then
if [ ! -d $PATH_QC/sim/$year ]; then
  echo "create dir"
  cp -r $PATH_QC/templateMC $PATH_QC/sim/$year
fi
rm $PATH_QC/sim/$year/periods
alien.py find /alice/$type/$year/$period/$pass qc_finalization.json|sed "s|qc_finalization.json|qc_archive.zip|g" >listaDone_$period
cat listaDone_$period |awk -F"/" '{print "./processReqMC.sh","/alice/sim/"$4"/"$5"/"$6,$7,$5,$4}'|bash

cd $PATH_QC/sim/$year/
./mkreportsMC.sh $period
cd -
fi
