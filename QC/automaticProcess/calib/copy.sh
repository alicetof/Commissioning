#!/bin/sh
./copySafe.sh $1 $2 $3 $4 $5

export instances=$(ps aux|grep alien.py|wc -l)

if [ $instances -gt 40 ]; then
sleep 20
fi
