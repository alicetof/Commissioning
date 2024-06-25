#! /usr/bin/env bash

PERIOD=$1
PASS=$2

. runQCasynch.sh $PERIOD $PASS

root -l -b -q makereportpbpb.cxx\(\"$PERIOD\",\"$PASS\"\)