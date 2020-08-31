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

inputfiles=(input/cent00*/*)
if [ -z "$2" ]
then
    njobs=${#inputfiles[@]}
else
    njobs=$2
fi

for (( i=0; i<$njobs; i++ ))
do
    n=$((i+1))
    nevents=$( echo ${inputfiles[$i]#*_n} | grep -o -E '[0-9]+' )
    outputdir=outputs/run_${1}_job$n
    mkdir $outputdir
    mkdir ${outputdir}/logs
    #idA=$(sbatch -o ${outputdir}/logs/log -e ${outputdir}/logs/errout -J simO2 --time=02:00:00 --ntasks 1 --cpus-per-task 12 run $outputdir ${inputfiles[$i]} $nevents | awk '{print $4}')
    #sbatch -o ${outputdir}/logs/log -e ${outputdir}/logs/errout -J simO2 --dependency=afterok:1297364 --ntasks 1 --cpus-per-task 12 run $outputdir ${inputfiles[$i]} $nevents
    sbatch -o ${outputdir}/logs/log -e ${outputdir}/logs/errout -J simO2 --ntasks 1 --cpus-per-task 12 run $outputdir ${inputfiles[$i]} $nevents
    sleep 1
done
