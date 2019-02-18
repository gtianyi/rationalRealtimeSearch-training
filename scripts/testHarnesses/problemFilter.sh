#!/bin/bash

if [ "$1" = "help" ] || [ "$1" = "-help" ] || [ "$1" = "?" ]
then
  echo "./problemFilter.sh <all file dir> <filter file> <target dir>"
  echo "all file dir:  slidingTile_rand1000"
  echo "filter file: random1000test-statSummary_300.txt"
  echo "target dir: slidingTile_hard300sec"
  exit 1
fi

if (($# < 3))
then
  echo "./problemFilter.sh <all file dir> <filter file> <target dir>"
  echo "all file dir:  slidingTile_rand1000"
  echo "filter file: random1000test-statSummary_300.txt"
  echo "target dir: slidingTile_300sec"
  exit 1
fi

allDir="../../../worlds/$1"

filterFile="../../../results/SlidingTilePuzzle/sampleData/$2"

targetDir="../../../worlds/$3"

outType=$4

mkdir -p ${targetDir}/hard
mkdir -p ${targetDir}/easy

fileCount=0

while IFS='' read -r line || [[ -n "$line" ]]; do

 if [[ "$line" == *"unsolved"* ]]; then
		 echo "$line"
		 continue
 fi

 fname=${line%.*}-4x4.st

 cp $allDir/$fname $targetDir/hard/
 let fileCount++

done < $filterFile

echo "${fileCount} hard files copied"

fileCount=0

for easyFile in $(ls $allDir); do
   if [ ! -f $targetDir/hard/$easyFile ]
   then
    let fileCount++
	cp $allDir/$easyFile $targetDir/easy/${fileCount}-4x4.st
   fi
done

echo "${fileCount} easy files copied"

fileCount=0

mkdir -p ${targetDir}/temp

for hardFile in $(ls $targetDir/hard); do
    let fileCount++
	cp $targetDir/hard/$hardFile $targetDir/temp/${fileCount}-4x4.st
done

rm -rf $targetDir/hard
mv $targetDir/temp $targetDir/hard





