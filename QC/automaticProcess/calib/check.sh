#!/bin/bash

timeout $1 sleep 3
export status=$?

if [ $status -gt 0 ]; then
echo failed
else
echo done
fi
