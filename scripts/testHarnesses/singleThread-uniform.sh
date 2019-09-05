#!/bin/bash

if [ "$1" = "help" ] || [ "$1" = "-help" ] || [ "$1" = "?" ]
then
  echo "./singleThread-uniform.sh <starting instance #> <# of instances to test> <search algorithm>"
  exit 1
fi

if (($# < 3))
then
  echo "./singleThread-uniform.sh <starting instance #> <# of instances to test> <search algorithm>"
  exit 1
fi

# Which instance to start testing on
firstInstance=$1

# The maximum number of instances to test on
maxInstances=$2
lastInstance=$(( $firstInstance + $maxInstances ))

searchalg=$3

mkdir -p ../../../results/SlidingTilePuzzle/sampleData/uniform
  instance=$firstInstance
  while ((instance < lastInstance))
  do
    file="../../../results/SlidingTilePuzzle/sampleProblem/uniform/${instance}.st"
      if [ -f ../../../results/SlidingTilePuzzle/sampleData/uniform/${instance}.txt ] || [ -f ../../../results/SlidingTilePuzzle/sampleData/uniform/${instance}.temp ]
	  then 
	    let instance++
	  else
		echo "t" > ../../../results/SlidingTilePuzzle/sampleData/uniform/${instance}.temp

	    ../../../build_release/tiles-uniform ${searchalg} uniform < ${file} > ../../../results/SlidingTilePuzzle/sampleData/uniform/${instance}.txt &

		if [ -f ../../../results/SlidingTilePuzzle/sampleData/uniform/${instance}.temp ]
		then
			rm ../../../results/SlidingTilePuzzle/sampleData/uniform/${instance}.temp
		fi

	    let instance++
	  fi
    done

