#!/bin/bash
export period=$1
export run=$2
ls accumulated/*|grep $run|awk -F"accumulated/" '{print "alien.py ls /alice/cern.ch/user/t/tofcalib/2024pb/'$period'/'$run'/"$2}'|bash
ls phase_$run*|awk '{print "alien.py ls /alice/cern.ch/user/t/tofcalib/2024pb/'$period'/'$run'/"$1}'|bash
