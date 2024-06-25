#!/bin/bash

cat periods |uniq|awk '{print "./mkperiod.sh",$1,"'$1'"}'|bash

