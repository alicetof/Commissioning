#!/bin/bash

ls TOF/Calib/LHCphase/|grep $1|awk -F"_" '{print $2","$3","$4}'|awk -F".root" '{print "root -b -q -l accumulate.C\\("$1"\\)"}'|bash
