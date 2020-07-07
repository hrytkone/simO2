#!/bin/bash

if [ "$1" == "help" ]
then
    echo "Usage: `basename $0` comment njobs[=no of input files]"
    exit 0
fi

if [ -z "$1" ]
then
    echo "Please give a comment to make this run unique (check '`basename $0` help' for help)"
    exit 0
fi

inputfiles=(input/centData/*)
if [ -z "$2" ]
then
    njobs=${#inputfiles[@]}
else
    njobs=$2
fi

for (( i=0; i<$njobs; i++ ))
do
    n=$((i+1))
    #nevents=$( echo ${inputfiles[$i]#*_n} | grep -o -E '[0-9]+' )
    nevents=443
    outputdir=outputs/run_${1}_job$n
    mkdir $outputdir
    mkdir ${outputdir}/logs
    #sbatch -o ${outputdir}/logs/log$n -e ${outputdir}/logs/errout$n -J simO2 -n 2 run $outputdir ${inputfiles[$i]} $nevents
    sbatch -o ${outputdir}/logs/log$n -e ${outputdir}/logs/errout$n -J simO2 -n 2 run $outputdir ${inputfiles[12]} $nevents
    sleep 1
done
