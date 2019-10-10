#!/bin/bash

if [ "$1" = "help" ] || [ "$1" = "-help" ] || [ "$1" = "?" ]
then
  echo "./singleThread-uniform.sh <starting instance #> <# of instances to test> <search algorithm> <collect algorithm>"
  exit 1
fi

if (($# < 4))
then
  echo "./singleThread-uniform.sh <starting instance #> <# of instances to test> <search algorithm> <collect algorithm>"
  exit 1
fi

# Which instance to start testing on
firstInstance=$1

# The maximum number of instances to test on
maxInstances=$2
lastInstance=$(( $firstInstance + $maxInstances ))

searchalg=$3

collectalg=$4

mkdir -p ../../../results/SlidingTilePuzzle/sampleData/uniform/${collectalg}
  instance=$firstInstance
  while ((instance < lastInstance))
  do
    file="../../../results/SlidingTilePuzzle/sampleProblem/uniform/${collectalg}/${instance}.st"
      if [ -f ../../../results/SlidingTilePuzzle/sampleData/uniform/${collectalg}/${instance}.txt ] || [ -f ../../../results/SlidingTilePuzzle/sampleData/uniform/${collectalg}/${instance}.temp ]
	  then 
	    let instance++
	  else
		echo "t" > ../../../results/SlidingTilePuzzle/sampleData/uniform/${collectalg}/${instance}.temp

	    ../../../build_release/tiles-uniform ${searchalg} uniform < ${file} > ../../../results/SlidingTilePuzzle/sampleData/uniform/${collectalg}/${instance}.txt 

		if [ -f ../../../results/SlidingTilePuzzle/sampleData/uniform/${collectalg}/${instance}.temp ]
		then
			rm ../../../results/SlidingTilePuzzle/sampleData/uniform/${collectalg}/${instance}.temp
		fi

	    let instance++
	  fi
    done

