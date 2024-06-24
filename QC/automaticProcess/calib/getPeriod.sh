#!/bin/bash
cat periods|grep $1|awk '{print $1}'
