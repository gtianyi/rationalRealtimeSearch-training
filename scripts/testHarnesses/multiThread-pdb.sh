#!/bin/bash

if [ "$1" = "help" ] || [ "$1" = "-help" ] || [ "$1" = "?" ]
then
  echo "./multiThread-pdb.sh <# of processes> <starting instance #> <# of instances to test> <search algorithm> <tile type> <training type>"
  exit 1
fi

if (($# < 6))
then
  echo "./multiThread-pdb.sh <# of processes> <starting instance #> <# of instances to test> <search algorithm> <tile type> <training type>"
  exit 1
fi

# Max number of background processes to start, should probably not be more than the number of cores on the machine
maxProcs=$1

# Which instance to start testing on
firstInstance=$2

# The maximum number of instances to test on
maxInstances=$3

searchalg=$4

tileType=$5

trainingType=$6

numProcs=0

trap "exit" INT

mkdir -p ../../../results/SlidingTilePuzzle/sampleData/${tileType}

while ((numProcs < ${maxProcs}))
do
    file="../../../results/SlidingTilePuzzle/sampleProblem/${tileType}/${trainingType}/${instance}.st"
	../../../build_release/tiles-pdb ${searchalg} ${tileType} ${firstInstance} ${maxInstances} &
	sleep 1
    let numProcs++
done
