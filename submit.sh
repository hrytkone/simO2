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

inputfiles=(input/cent40*/*)
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
    nevents=1
    outputdir=outputs/run_${1}_job$n
    mkdir $outputdir
    mkdir ${outputdir}/logs
    #idA=$(sbatch -o ${outputdir}/logs/log -e ${outputdir}/logs/errout -J simO2 --time=02:00:00 -n 12 --exclusive run $outputdir ${inputfiles[$i]} $nevents | awk '{print $4}')
    #sbatch -o ${outputdir}/logs/log -e ${outputdir}/logs/errout -J simO2 --dependency=afterok:1297364 -n 12 --exclusive run $outputdir ${inputfiles[$i]} $nevents
    sbatch -o ${outputdir}/logs/log -e ${outputdir}/logs/errout -J bgtest -n 12 --exclusive run $outputdir ${inputfiles[$i]} $nevents
    sleep 1
done
