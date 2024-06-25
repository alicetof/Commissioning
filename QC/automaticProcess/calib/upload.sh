#!/bin/bash
export CURRENT_YEAR=${CURRENT_YEAR:-2024}
export period=$1
export run=$2
alien.py mkdir /alice/cern.ch/user/t/tofcalib/$CURRENT_YEAR/$period
alien.py mkdir /alice/cern.ch/user/t/tofcalib/$CURRENT_YEAR/$period/$run
ls accumulated/*|grep $run|awk '{print "alien.py cp file:"$1,"/alice/cern.ch/user/t/tofcalib/'$CURRENT_YEAR'/'$period'/'$run'"}'|bash
ls phase_$run*|awk '{print "alien.py cp file:"$1,"/alice/cern.ch/user/t/tofcalib/'$CURRENT_YEAR'/'$period'/'$run'"}'|bash
