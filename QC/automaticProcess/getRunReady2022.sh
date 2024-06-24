#!/bin/bash
alien.py find /alice/data/2022/ QC_fullrun.root |grep "$1/"|awk -F"/" '{print $5,$6}' >listaDone_$1

