#! /usr/bin/env bash

RUN="Run505285"
SLOT="1635384636541"
HITS="1635384636658"
NOISE="1635384636639"
CHANNELS="1635384636589"
RATE="1635384637184"
HITMAP="1635384636261"
MULTI="1635384636222"
ORBITDDL="1635384636274"
RAWTIME="1635384636236"
TREND="1635384640563"

python3 fetch_output.py qc/TOF/MO/TaskRaw/hSlotPartMask -t $SLOT
mv $RUN/qc/TOF/MO/TaskRaw/hSlotPartMask/snapshot.root  $RUN/hSlotPartMask.root

python3 fetch_output.py qc/TOF/MO/TaskRaw/hHits -t $HITS
mv $RUN/qc/TOF/MO/TaskRaw/hHits/snapshot.root  $RUN/hHits.root

python3 fetch_output.py qc/TOF/MO/TaskRaw/hIndexEOIsNoise -t $NOISE
mv $RUN/qc/TOF/MO/TaskRaw/hIndexEOIsNoise/snapshot.root  $RUN/hIndexEOIsNoise.root

python3 fetch_output.py qc/TOF/MO/TaskRaw/hIndexEOInTimeWin -t $CHANNELS
mv $RUN/qc/TOF/MO/TaskRaw/hIndexEOInTimeWin/snapshot.root  $RUN/hIndexEOInTimeWin.root

python3 fetch_output.py qc/TOF/MO/TaskRaw/hIndexEOHitRate -t $RATE
mv $RUN/qc/TOF/MO/TaskRaw/hIndexEOHitRate/snapshot.root  $RUN/hIndexEOHitRate.root

python3 fetch_output.py qc/TOF/MO/TaskDigits/TOFRawHitMap -t $HITMAP
mv $RUN/qc/TOF/MO/TaskDigits/TOFRawHitMap/snapshot.root  $RUN/TOFRawHitMap.root

python3 fetch_output.py qc/TOF/MO/TaskDigits/TOFRawsMulti -t $MULTI
mv $RUN/qc/TOF/MO/TaskDigits/TOFRawsMulti/snapshot.root  $RUN/TOFRawsMulti.root

python3 fetch_output.py qc/TOF/MO/TaskDigits/OrbitDDL -t $ORBITDDL
mv $RUN/qc/TOF/MO/TaskDigits/OrbitDDL/snapshot.root  $RUN/OrbitDDL.root

python3 fetch_output.py qc/TOF/MO/TaskDigits/TOFRawsTime -t $ORBITDDL
mv $RUN/qc/TOF/MO/TaskDigits/TOFRawsTime/snapshot.root  $RUN/TOFRawsTime.root

python3 fetch_output.py qc/TOF/MO/TOFTrendingHits/mean_of_hits -t $TREND
mv $RUN/qc/TOF/MO/TOFTrendingHits/mean_of_hits/snapshot.root  $RUN/mean_of_hits.root

rm -rf $RUN/qc
	