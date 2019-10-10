#!/bin/bash

if [ "$1" = "help" ] || [ "$1" = "-help" ] || [ "$1" = "?" ]
then
  echo "./singleThread-distributionExpHarness.sh <starting instance #> <# of instances to test> <Puzzle Type> <alg> <alg para> "
  echo "Available puzzle types are uniform heavy"
  echo "Available algorithms are wastar lsslrtastar"
  echo "algorithm parameters for wastar: weight, lsslrtastar: lookahead"
  exit 1
fi

if (($# < 5))
then
  echo "./singleThread-distributionExpHarness.sh <starting instance #> <# of instances to test> <Puzzle Type> <alg> <alg para> "
  echo "Available puzzle types are uniform heavy"
  echo "Available algorithms are wastar lsslrtastar"
  echo "algorithm parameters for wastar: weight, lsslrtastar: lookahead"
  exit 1
fi

# Which instance to start testing on
firstInstance=$1

# The maximum number of instances to test on
maxInstances=$2
lastInstance=$(( $firstInstance + $maxInstances ))

# The domain to run on
domainType=$3

# algorithm
algType=$4

algPara=$5

mkdir -p ../../../results/SlidingTilePuzzle/distributionTest/${domainType}/${algType}
  instance=$firstInstance
  while ((instance < lastInstance))
  do
    file="../../../worlds/slidingTile/${instance}-4x4.st"
    if [ -f ../../../results/SlidingTilePuzzle/distributionTest/${domainType}/${algType}/Para${algPara}-${instance}.txt ] || [ -f ../../../results/SlidingTilePuzzle/distributionTest/${domainType}/${algType}/Para${algPara}-${instance}.temp ]
	then 
	  let instance++
	else
      echo "t" > ../../../results/SlidingTilePuzzle/distributionTest/${domainType}/${algType}/Para${algPara}-${instance}.temp

      ./../../../build_release/distributionPractice ${domainType} ${algType} ${algPara} ../../../results/SlidingTilePuzzle/distributionTest/${domainType}/${algType}/Para${algPara}-${instance}.txt < ${file} 

	  if [ -f ../../../results/SlidingTilePuzzle/distributionTest/${domainType}/${algType}/Para${algPara}-${instance}.temp ]
      then
		rm ../../../results/SlidingTilePuzzle/distributionTest/${domainType}/${algType}/Para${algPara}-${instance}.temp
      fi
	  
	  let instance++
	fi
  done

