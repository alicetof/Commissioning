#!/bin/bash
export period=$1
export run=$2

ls $run/calibTobeKept.root|awk '{print "alien.py cp file:"$1,"/alice/cern.ch/user/t/tofcalib/'$CURRENT_YEAR'/'$period'/'$run'"}'|bash
