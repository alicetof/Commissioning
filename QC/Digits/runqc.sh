#!/bin/bash

o2-tof-reco-workflow -b --output-type none \
           | \
           o2-qc -b --config json://${PWD}/tof.json 



