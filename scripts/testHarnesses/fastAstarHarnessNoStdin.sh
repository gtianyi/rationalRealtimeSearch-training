#!/bin/bash

if [ "$1" = "help" ] || [ "$1" = "-help" ] || [ "$1" = "?" ]
then
  echo "./fastAstarHarness.sh <starting instance #> <# of instances to test> <# of processes> <tile type> <search algorithm>"
  exit 1
fi

if (($# < 4))
then
  echo "./fastAstarHarness.sh <starting instance #> <# of instances to test> <# of processes> <tile type> <search algorithm>"
  exit 1
fi

# Which instance to start testing on
firstInstance=$1

# The maximum number of instances to test on
maxInstances=$2

# Max number of background processes to start, should probably not be more than the number of cores on the machine
maxProcs=$3

tileType=$4

searchalg=$5

numProcs=0

mkdir -p ../../../results/SlidingTilePuzzle/sampleData/${tileType}
while ((numProcs < ${maxProcs}))
do
	../../../build_release/tiles-${tileType} ${searchalg} ${tileType} $firstInstance $maxInstances &
    let numProcs++
done

