#!/bin/bash
export run=$2
export period=$1

export ts=$(cat timestamps|grep $run|awk '{print $2}')
o2-ccdb-downloadccdbfile -p TOF/Calib/FEELIGHT -t $ts
o2-ccdb-downloadccdbfile -p TOF/Calib/ChannelCalib -t $ts

# download QC with hit map
rm QC.root
alien.py find /alice/data/2025/$period/$run/cpass0/ QC_fullrun.root|awk '{print "alien.py cp",$1,"file:QC.root"}'|bash

root -b -q -l checkmapqc.C\($run,\"$period\"\)
