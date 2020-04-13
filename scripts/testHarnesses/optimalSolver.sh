#!/bin/bash

print_usage(){
  echo "./singleThread-uniform.sh"
  echo "[-f instance]                    default: 1"
  echo "[-n # of instances to test]      default: 100"
  echo "[-d domain]                      default: pancake"
  echo "[-s subdomain]                   default: regular"
  echo "[-z domain size]                 default: 16"
  echo "[-o optimal solver]              default: distributionPractice"
  echo "[-u suboptimal solver]           default: wastar"
  echo "[-p suboptimal solver parameter] default: 2"
  echo "[-t thread number]               default: 1"
  echo "[-h help]"
  exit 1
}

if [ "$1" = "help" ] || [ "$1" = "-help" ] || [ "$1" = "?" ]; then
  print_usage
fi

# Which instance to start testing on
first=1
# The number of instances to test on
n_of_i=100
optimal_solver="distributionPractice"
domain="pancake"
subdomain="regular"
size="16"
suboptimal_solver="wastar"
suboptimal_para="2"
maxProcs=1

#parse arguments
for (( i=1; i <= "$#"; i++ )); do
    if [ ${!i} == "-f" ]; then
        if [ $((i+1)) -le "$#" ]; then
            var=$((i+1))
            first=${!var}
        fi
    fi

    if [ ${!i} == "-n" ]; then
        if [ $((i+1)) -le "$#" ]; then
            var=$((i+1))
            n_of_i=${!var}
        fi
    fi

    if [ ${!i} == "-d" ]; then
        if [ $((i+1)) -le "$#" ]; then
            var=$((i+1))
			domain=${!var}
        fi
    fi

    if [ ${!i} == "-s" ]; then
        if [ $((i+1)) -le "$#" ]; then
            var=$((i+1))
			subdomain=${!var}
        fi
    fi

    if [ ${!i} == "-z" ]; then
        if [ $((i+1)) -le "$#" ]; then
            var=$((i+1))
			size=${!var}
        fi
    fi

    if [ ${!i} == "-o" ]; then
        if [ $((i+1)) -le "$#" ]; then
            var=$((i+1))
			optimal_solver=${!var}
        fi
    fi

    if [ ${!i} == "-u" ]; then
        if [ $((i+1)) -le "$#" ]; then
            var=$((i+1))
			suboptimal_solver=${!var}
        fi
    fi

    if [ ${!i} == "-p" ]; then
        if [ $((i+1)) -le "$#" ]; then
            var=$((i+1))
			suboptimal_para=${!var}
        fi
    fi

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

echo "first ${first}"
echo "n_of_i ${n_of_i}"
echo "domain ${domain}"
echo "subdomain ${subdomain}"

solver_command=""

if [ "$optimal_solver" == "distributionPractice" ]; then
  solver_command="distributionPractice -d ${domain} -s ${subdomain} -a wastar -p 1"
elif [ "$optimal_solver" == "tiles-uniform" ]; then
  solver_command="tiles-uniform idastar uniform"
else
  echo "unknow solver"
  exit 1
fi

last=$(( $first + $n_of_i ))

outfile_dir="../../../results/${domain}/sampleData/${subdomain}/\
${suboptimal_solver}/${suboptimal_para}/${size}"

infile_dir="../../../results/${domain}/sampleProblem/${subdomain}/\
${suboptimal_solver}/Para${suboptimal_para}/${size}"

mkdir -p ${outfile_dir}

numProcs=0

#stop all if hit ctrl+c
trap "exit" INT

while ((numProcs < ${maxProcs}))
do

  instance=$first
  while ((instance < last))
  do
    infile="${infile_dir}/${instance}.st" 
    outfile_name="${outfile_dir}/${instance}"
	outfile="${outfile_name}.txt"
	tempfile="${outfile_name}.temp"

    if [ -f ${outfile} ] || [ -f ${tempfile} ]; then 
	    let instance++
	else
        echo "t" > ${tempfile} 

	    ../../../build_release/${solver_command} < ${infile} > ${outfile} 

	    if [ -f ${tempfile} ]; then
		  rm ${tempfile}
        fi
	
	    let instance++
	  fi
  done

  sleep 1
  let numProcs++

done
