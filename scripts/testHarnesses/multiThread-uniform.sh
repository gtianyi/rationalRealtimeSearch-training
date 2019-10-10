#!/bin/bash

if [ "$1" = "help" ] || [ "$1" = "-help" ] || [ "$1" = "?" ]
then
  echo "./multiThread-uniform.sh <# of processes> <starting instance #> <# of instances to test> <search algorithm> <collect algorithm>"
  exit 1
fi

if (($# < 5))
then
  echo "./multiThread-uniform.sh <# of processes> <starting instance #> <# of instances to test> <search algorithm> <collect algorithm>"
  exit 1
fi

# Max number of background processes to start, should probably not be more than the number of cores on the machine
maxProcs=$1

numProcs=0

trap "exit" INT

while ((numProcs < ${maxProcs}))
do
	./singleThread-uniform.sh ${@:2} &
	sleep 1
    let numProcs++
done
