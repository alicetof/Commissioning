#! /usr/bin/env bash

RUN=$1
TIMESTAMP=$2
echo "-------------------------------------"
echo "  Providing QC plots for Run $RUN   "
echo "-------------------------------------"

rm -rf Run$RUN/
mkdir Run$RUN
mkdir Run$RUN/rootfiles

# Declare an array of string with type
declare -a MatchingMONames=( "mEffEta_ITSTPC-ITSTPCTRD" "mEffEta_TPC" "mEffPt_ITSTPC-ITSTPCTRD" "mEffPt_TPC" "mDeltaXEtaITSTPC-ITSTPCTRD"
                             "mDeltaXEtaTPC" "mDeltaZEtaITSTPC-ITSTPCTRD" "mDeltaZEtaTPC" "mTOFChi2ITSTPC-ITSTPCTRD" "mTOFChi2TPC" )
declare -a DigitsMONames=( "HitMap" )
declare -a PIDMONames=( "BetavsP" "HadronMasses" "DeltatPi" )

# Iterate the string array using for loop
for name in ${MatchingMONames[@]}; do
    python3 fetch_output.py qc_async/TOF/MO/MatchingTOF/$name -t $TIMESTAMP -o Run$RUN
    mv Run$RUN/qc_async/TOF/MO/MatchingTOF/$name/snapshot.root  Run$RUN/rootfiles/$name.root
done

for name in ${DigitsMONames[@]}; do
    declare -a newname=$(echo $name | tr / _)
    python3 fetch_output.py qc_async/TOF/MO/TaskDigits/$name -t $TIMESTAMP -o Run$RUN
    mv Run$RUN/qc_async/TOF/MO/TaskDigits/$name/snapshot.root  Run$RUN/rootfiles/$newname.root
done

for name in ${PIDMONames[@]}; do
    python3 fetch_output.py qc_async/TOF/MO/TaskFT0TOF/$name -t $TIMESTAMP -o Run$RUN
    mv Run$RUN/qc_async/TOF/MO/TaskFT0TOF/$name/snapshot.root  Run$RUN/rootfiles/$name.root
done

rm -rf Run$RUN/qc