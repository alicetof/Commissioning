#!/bin/bash
rm jiratext.txt x*
cat $MAIN_PATH/listaDone_$2 |grep $1|awk '{print "./addrun.sh",$2}' |bash
echo $1
cat jiratext.txt
split jiratext.txt -l 30
ls x*|cat -b|awk '{print "./mksubperiod.sh '$1' '$2'",$1,$2}'|bash
mv *.pdf eos/.
