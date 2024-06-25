#!/bin/bash
source config

while [ 1 ]
do
  ./processAllFromPass.sh cpass0
  ./processAllFromPassFull.sh apass1
  cat requests |grep 20|awk '{print "./mkrequests.sh",$1,$2,$3,$4}'|bash
  sleep 600
done
