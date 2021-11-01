#! /usr/bin/env bash

RUN="505720"
TIMESTAMP="1635741378837"

mkdir Run$RUN

python3 fetch_output.py qc/TOF/MO/TaskRaw/hSlotPartMask -t $TIMESTAMP -o ./Run$RUN
mv Run$RUN/qc/TOF/MO/TaskRaw/hSlotPartMask/snapshot.root  Run$RUN/hSlotPartMask.root

python3 fetch_output.py qc/TOF/MO/TaskRaw/hHits -t $TIMESTAMP -o ./Run$RUN
mv Run$RUN/qc/TOF/MO/TaskRaw/hHits/snapshot.root  Run$RUN/hHits.root

python3 fetch_output.py qc/TOF/MO/TaskRaw/hIndexEOIsNoise -t $TIMESTAMP -o ./Run$RUN
mv Run$RUN/qc/TOF/MO/TaskRaw/hIndexEOIsNoise/snapshot.root  Run$RUN/hIndexEOIsNoise.root

python3 fetch_output.py qc/TOF/MO/TaskRaw/hIndexEOInTimeWin -t $TIMESTAMP -o ./Run$RUN
mv Run$RUN/qc/TOF/MO/TaskRaw/hIndexEOInTimeWin/snapshot.root  Run$RUN/hIndexEOInTimeWin.root

python3 fetch_output.py qc/TOF/MO/TaskRaw/hIndexEOHitRate -t $TIMESTAMP -o ./Run$RUN
mv Run$RUN/qc/TOF/MO/TaskRaw/hIndexEOHitRate/snapshot.root  Run$RUN/hIndexEOHitRate.root

python3 fetch_output.py qc/TOF/MO/TaskDigits/TOFRawHitMap -t $TIMESTAMP -o ./Run$RUN
mv Run$RUN/qc/TOF/MO/TaskDigits/TOFRawHitMap/snapshot.root  Run$RUN/TOFRawHitMap.root

python3 fetch_output.py qc/TOF/MO/TaskDigits/TOFRawsMulti -t $TIMESTAMP -o ./Run$RUN
mv Run$RUN/qc/TOF/MO/TaskDigits/TOFRawsMulti/snapshot.root  Run$RUN/TOFRawsMulti.root

python3 fetch_output.py qc/TOF/MO/TaskDigits/OrbitDDL -t $TIMESTAMP -o ./Run$RUN
mv Run$RUN/qc/TOF/MO/TaskDigits/OrbitDDL/snapshot.root  Run$RUN/OrbitDDL.root

python3 fetch_output.py qc/TOF/MO/TaskDigits/TOFRawsTime -t $TIMESTAMP -o ./Run$RUN
mv Run$RUN/qc/TOF/MO/TaskDigits/TOFRawsTime/snapshot.root  Run$RUN/TOFRawsTime.root

python3 fetch_output.py qc/TOF/MO/TOFTrendingHits/mean_of_hits -t $TIMESTAMP -o ./Run$RUN
mv Run$RUN/qc/TOF/MO/TOFTrendingHits/mean_of_hits/snapshot.root  Run$RUN/mean_of_hits.root

root -l draw.C\(\"$RUN\"\)

rm -rf Run$RUN/qc
	