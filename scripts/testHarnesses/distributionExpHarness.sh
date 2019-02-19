#!/bin/bash

if [ "$1" = "help" ] || [ "$1" = "-help" ] || [ "$1" = "?" ]
then
  echo "./distributionExpHarness.sh <starting instance #> <# of instances to test> <# of processes> <Puzzle Type> <Weight> "
  echo "Available puzzle types are Uniform heavy"
  echo "Domain variables for SlidingPuzzle: weight"
  exit 1
fi

if (($# < 5))
then
  echo "./distributionExpHarness.sh <starting instance #> <# of instances to test> <# of processes> <Puzzle Type> <Weight> "
  echo "Available puzzle types are Uniform heavy"
  echo "Domain variables for SlidingPuzzle: weight"
  exit 1
fi

# Which instance to start testing on
firstInstance=$1

# The maximum number of instances to test on
maxInstances=$2
lastInstance=$(( $firstInstance + $maxInstances ))

# Max number of background processes to start, should probably not be more than the number of cores on the machine
maxProcs=$3

# The domain to run on
domainType=$4

weight=$5

numProcs=0

mkdir -p ../../../results/SlidingTilePuzzle/distributionTest/${domainType}
  instance=$firstInstance
  while ((instance < lastInstance))
  do
    file="../../../worlds/slidingTile_300sec/easy/${instance}-4x4.st"
    if ((numProcs >= ${maxProcs}))
      then
        wait
        numProcs=0
      fi		  
      if [ -f ../../../results/SlidingTilePuzzle/distributionTest/${domainType}/W${weight}-${instance}.txt ]
	  then 
	    let instance++
	  else
		  ./../../../build_release/distributionPractice ${domainType} ${weight} ../../../results/SlidingTilePuzzle/distributionTest/${domainType}/W${weight}-${instance}.txt < ${file} &
	    let instance++
        let numProcs++
	  fi
    done

