#!/bin/bash

print_usage(){
  echo "./singleThread-uniform.sh"
  echo "[-f instance]                    default: 1"
  echo "[-n # of instances to test]      default: 1"
  echo "[-d domain]                      default: racetrack"
  echo "[-s subdomain]                   default: barto-bigger"
  echo "[-z domain size]                 default: -1"
  echo "[-o optimal solver]              default: wastar"
  echo "[-u suboptimal solver]           default: wastar"
  echo "[-p suboptimal solver parameter] default: 2"
  echo "[-t thread number]               default: 1"
  echo "[-h help]"
  exit 1
}

if [ "$1" = "-h" ] || [ "$1" = "-help" ] || [ "$1" = "?" ]; then
  print_usage
fi

maxProcs=1

# parse -t arguments
# all arguments would be pass to singleThread
for (( i=1; i <= "$#"; i++ )); do

    if [ ${!i} == "-t" ]; then
        if [ $((i+1)) -le "$#" ]; then
            var=$((i+1))
			maxProcs=${!var}
        fi
    fi

    if [ ${!i} == "-h" ]; then
		print_usage
    fi

done

echo "thread ${maxProcs}"

#stop all if hit ctrl+c
trap "exit" INT

numProcs=0
while ((numProcs < ${maxProcs}))
do
  ./singleThread-optimalSolver.sh ${@} &
  sleep 1
  let numProcs++
done
