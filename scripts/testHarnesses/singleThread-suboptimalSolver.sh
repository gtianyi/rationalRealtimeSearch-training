#!/bin/bash
print_usage(){
  echo "./singleThread-suboptmialSolver.sh"
  echo "[-f instance]                    default: 1"
  echo "[-n # of instances to test]      default: 1"
  echo "[-d domain]                      default: racetrack"
  echo "[-s subdomain]                   default: barto-bigger"
  echo "[-z domain size]                 default: -1"
  echo "[-u suboptimal solver]           default: wastar"
  echo "[-p suboptimal solver parameter] default: 2"
  echo "[-h help]"
  exit 1
}

if [ "$1" = "-h" ] || [ "$1" = "-help" ] || [ "$1" = "?" ]; then
  print_usage
fi

# Which instance to start testing on
first=1
# The number of instances to test on
n_of_i=1
domain="racetrack"
subdomain="barto-bigger"
size="-1"
suboptimal_solver="wastar"
suboptimal_para="2"

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

    if [ ${!i} == "-h" ]; then
		print_usage
    fi

done

echo "first ${first}"
echo "n_of_i ${n_of_i}"
echo "domain ${domain}"
echo "subdomain ${subdomain}"
echo "size ${size}"

infile=""
outfile=""

infile_path="../../../worlds/${domain}"
outfile_path="../../../results/${domain}/distributionTest/${subdomain}/${suboptimal_solver}"

mkdir -p ${outfile_path}

if [ "$domain" == "tile" ]; then
  infile="${infile_path}/instance-${size}x${size}.st"
  outfile="${outfile_path}/Para${suboptimal_para}-${size}-instance.txt"
fi

if [ "$domain" == "pancake" ]; then
  infile="${infile_path}/instance-${size}.pan"
  outfile="${outfile_path}/Para${suboptimal_para}-${size}-instance.txt"
fi

if [ "$domain" == "racetrack" ]; then
  infile="${infile_path}/${subdomain}-instance.init"
  outfile="${outfile_path}/Para${suboptimal_para}-${subdomain}-instance.txt"
fi

last=$(( $first + $n_of_i ))

instance=$first
while ((instance < last))
do
    infile_instance="${infile/instance/$instance}"
	outfile_instance="${outfile/instance/$instance}"
	tempfile="${outfile_instance}.temp"

    if [ -f ${outfile_instance} ] || [ -f ${tempfile} ]; then 

	  let instance++

	else

      echo "t" > ${tempfile} 

      ./../../../build_release/distributionPractice \
			  -d ${domain} -s ${subdomain} -a ${suboptimal_solver} \
			  -p ${suboptimal_para}  -o ${outfile_instance} < ${infile_instance} 

	  if [ -f ${tempfile} ]; then
		rm ${tempfile}
      fi
	  
	  let instance++

	fi

done

