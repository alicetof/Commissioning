#!/bin/bash
find .|grep rootfiles|grep -v "rootfiles/" >runs
cat runs|awk '{print "./checkSingle.sh",$1}' |bash
find .|grep Run|grep -v rootfiles >runs
cat runs|awk '{print "./checkSingle2.sh",$1}'|bash
