#!/bin/bash

if [ "$1" = "help" ] || [ "$1" = "-help" ] || [ "$1" = "?" ]
then
  echo "./fastAstarHarness.sh <starting instance #> <# of instances to test> <# of processes>"
  exit 1
fi

if (($# < 2))
then
  echo "./fastAstarHarness.sh <starting instance #> <# of instances to test> <# of processes>"
  exit 1
fi

# Which instance to start testing on
firstInstance=$1

# The maximum number of instances to test on
maxInstances=$2
lastInstance=$(( $firstInstance + $maxInstances ))

# Max number of background processes to start, should probably not be more than the number of cores on the machine
maxProcs=$3

numProcs=0

mkdir -p ../../../results/SlidingTilePuzzle/sampleData/random1000test
  instance=$firstInstance
  while ((instance < lastInstance))
  do
    file="../../../worlds/slidingTile_hard900sec/${instance}-4x4.st"

    if [ ! -f $file ]; then
		continue
    fi

    if ((numProcs >= ${maxProcs}))
      then
        wait
        numProcs=0
      fi		  
      if [ -f ../../../results/SlidingTilePuzzle/sampleData/randdom1000test/${instance}.txt ]
	  then 
	    let instance++
	  else
	    timeout 1800 ../../../build_release/tiles idastar uniform < ${file} > ../../../results/SlidingTilePuzzle/sampleData/random1000test/${instance}.txt &
	    let instance++
	  fi
    done

