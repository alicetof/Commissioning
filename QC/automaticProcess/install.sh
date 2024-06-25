#!/bin/bash
source config

mkdir -p $MAIN_PATH
cp * $MAIN_PATH/.

mkdir -p $PATH_QC/2024
mkdir -p $PATH_CALIB
mkdir -p $PATH_CERNBOX

mkdir template/eos
mkdir templateMC/eos
cp -r template $PATH_QC/.
cp -r templateMC $PATH_QC/.
cp -r template $PATH_QC/2024/cpass0
cp -r template $PATH_QC/2024/apass1
cp calib/* $PATH_CALIB/.
cp QC/* $PATH_QC/.

cp cernbox/* ~

sed -i "s|PATH_TO_CONFIG|$MAIN_PATH|g" ~/syncQC.sh
sed -i "s|PATH_TO_CONFIG|$MAIN_PATH|g" ~/mountcernbox
sed -i "s|PATH_TO_CONFIG|$MAIN_PATH|g" ~/umountcernbox

# add to cron
export isAlready=$(crontab -l|grep syncQC|wc -l)
if [ $isAlready -lt 1 ]; then
(crontab -l; echo "0,30 * * * * ~/syncQC.sh") | crontab -
fi
