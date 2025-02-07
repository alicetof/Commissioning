#!/bin/bash
rm jiratext.txt x*
cat $MAIN_PATH/listaDone_$2 |grep $1|awk -F"/" '{print "./addrun.sh",$7,"'$2'"}' |bash
echo $1
cat jiratext.txt
split jiratext.txt -l 30
ls x*|cat -b|awk '{print "./mksubperiod.sh '$1' '$2'",$1,$2,"'$3' '$4'"}'|bash
mv *.pdf eos/.
