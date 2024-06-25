#!/bin/bash

ls accumulated/$1*.root >listaCal$1
root -b -q -l doCal.C+\($1\)

