#!/bin/bash

root -b -q -l calibPhase.C\($1\)
#cp ../pbpb23/phase_$1.txt .
#cp ../pbpb23/TOF/Calib/LHCphase/o2-dataformats-CalibLHCphaseTOF_$1* TOF/Calib/LHCphase/.

ls TOF/Calib/LHCphase/|grep $1|awk -F"_" '{print $2","$3","$4}'|awk -F".root" '{print "root -b -q -l accumulate.C\\("$1"\\)"}'|bash
