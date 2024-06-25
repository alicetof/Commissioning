#!/bin/bash

export nobjs=$(ls Run$1/rootfiles/|wc -l)
echo $1 -> $nobjs
if [ $nobjs -gt 23 ]; then
echo $1 >>jiratext.txt
fi
