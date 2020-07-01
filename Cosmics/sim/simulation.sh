#! /usr/bin/env bash

DIR=$(dirname "${BASH_SOURCE[0]}")
[ -z "$1" ] && NEVENTS=1000 || NEVENTS=$1

echo " --- running simulation: ${NEVENTS} events "
echo " --- external generator: ${DIR}/cosmicGenerator.C "

o2-sim-serial -n ${NEVENTS} -g extgen -m PIPE TOF --field 0 --extGenFile ${DIR}/cosmicGenerator.C
