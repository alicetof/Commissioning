#!/bin/bash
cat runs|grep 5|awk '{print "./run.sh",$1}'|bash
./adjust.sh
