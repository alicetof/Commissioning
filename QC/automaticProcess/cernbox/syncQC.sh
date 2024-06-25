#!/bin/bash

source PATH_TO_CONFIG/config

if [ -d $PATH_CERNBOX/QC/cpass0 ]; then
echo sync
rsync -avPzh $PATH_QC/$CURRENT_YEAR/cpass0/eos/* $PATH_CERNBOX/QC/cpass0/.
rsync -avPzh $PATH_QC/$CURRENT_YEAR/apass1/eos/* $PATH_CERNBOX/QC/apass1/.
rsync -avPzh $PATH_CALIB/$CURRENT_YEAR/TOF/Calib/* $PATH_CERNBOX/QC/Calib/.

cat $MAIN_PATH/requests |grep 20|grep data|awk '{print "mkdir '$PATH_CERNBOX'/QC/"$2"; mkdir '$PATH_CERNBOX'/QC/"$2"/"$4}'|bash
cat $MAIN_PATH/requests |grep 20|grep data|awk '{print "rsync -avPzh '$PATH_QC'/"$2"/"$4"/eos/* '$PATH_CERNBOX'/QC/"$2"/"$4"/."}'|bash
cat $MAIN_PATH/requests |grep 20|grep sim|awk '{print "mkdir '$PATH_CERNBOX'/QC/MC/"$2}'|bash
cat $MAIN_PATH/requests |grep 20|grep sim|awk '{print "rsync -avPzh '$PATH_QC'/sim/"$2"/eos/* '$PATH_CERNBOX'/QC/MC/"$2"/."}'|bash
fi

