#!/bin/bash
cp $4 jiratext.txt
root -b -q -l  makereportpbpb.cxx\(\"$1\",\"$2\"\)
root -b -q -l compareMCData.cxx\(\"$5\",\"$6\",\"$1\",$3\)
ls *.pdf|grep -v Run|awk -F".pdf" '{print "mv",$1".pdf",$1"_part'$3'.pdf"}'|bash
mv *.pdf eos/.
