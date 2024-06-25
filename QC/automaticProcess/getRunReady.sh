#!/bin/bash
alien.py find /alice/data/$CURRENT_YEAR/ QC_fullrun.root |grep -v LHC24aa|grep -v LHC24ab |grep -v LHC24ac|grep -v LHC24ad|grep -v LHC24ae|grep -v LHC24ah|grep "$1/"|awk -F"/" '{print $5,$6}' >listaDone_$1

