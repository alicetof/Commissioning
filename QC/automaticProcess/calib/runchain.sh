#!/bin/bash

./collect.sh $1 $2 cpass0
./doPhase_TS.sh $2
./calibOffset.sh $2
