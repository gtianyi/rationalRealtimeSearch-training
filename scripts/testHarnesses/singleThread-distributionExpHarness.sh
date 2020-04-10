#!/bin/bash

print_usage () {
  echo "./singleThread-distributionExpHarness.sh <starting instance #> <# of instances to test> <domain type> <subdomain type> <alg> <alg para> "
  echo "Available domain types are tile, pancake"
  echo "Available tile subdomain ypes are uniform heavy"
  echo "Available pancake subdomain ypes are regular heavy"
  echo "Available algorithms are wastar lsslrtastar"
  echo "algorithm parameters for wastar: weight, lsslrtastar: lookahead"
  exit 1
}

if [ "$1" = "help" ] || [ "$1" = "-help" ] || [ "$1" = "?" ] || (($# < 6))
then
  print_usage
fi

# Which instance to start testing on
firstInstance=$1

# The maximum number of instances to test on
maxInstances=$2
lastInstance=$(( $firstInstance + $maxInstances ))

# The domain to run on
domainType=$3

# The sub domain to run on
subDomainType=$4

# algorithm
algType=$5

algPara=$6

domainsize="16"
ext="pan"

if [ "$domainType" == "tile" ]; then

  domainsize="4x4"
  ext="st"

fi

mkdir -p ../../../results/${domainType}/distributionTest/${subDomainType}/${algType}
instance=$firstInstance
while ((instance < lastInstance))
do
    infile="../../../worlds/${domainType}/${instance}-${domainsize}.${ext}"
	outfile_name="../../../results/${domainType}/distributionTest/${subDomainType}/\
${algType}/Para${algPara}-${instance}-${domainsize}"
	outfile="${outfile_name}.txt"
	tempfile="${outfile_name}.temp"

    if [ -f ${outfile} ] || [ -f ${tempfile} ]; then 

	  let instance++

	else

      echo "t" > ${tempfile} 

      ./../../../build_release/distributionPractice \
			  -d ${domainType} -s ${subDomainType} -a ${algType} \
			  -p ${algPara}  -o ${outfile} < ${infile} 

	  if [ -f ${tempfile} ]; then
		rm ${tempfile}
      fi
	  
	  let instance++

	fi

done

