#!/bin/bash

#usage: ./RunNSimulations.sh -n <NumberOfSimulations> -f <finalTime>    -->     ./RunNSimulations.sh -n 4 -f 1

# Parameters
numberOfSimulations=-1
finalTime=-1


# Read command arguments
while getopts ":f:n:" optname
  do
    case "$optname" in
      "f")
	finalTime=${OPTARG}
        #echo "finalTime set = $finalTime"
        ;;      
     "n")
       	numberOfSimulations=${OPTARG}
	#echo "numberOfSimulations set = $numberOfSimulations"
        ;;      
      *)
      # Should not occur
        echo "Unknown option $optname"
        ;;
    esac    
done

echo "------ Executing simulations ${numberOfSimulations} Simulations. finalTime=${finalTime}----------"
echo 

for ((n=0;n<$numberOfSimulations;n++))
do
  command="./model -tf ${finalTime}"
  echo "Iteration $n: $command"
  $command
  echo
done


