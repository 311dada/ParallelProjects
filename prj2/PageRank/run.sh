#!/bin/bash

make clean
make

multiHosts=0
graph=0

while getopts "n:f:t:e:" arg 
do
        case $arg in
             n)
                n=$OPTARG
                ;;
             f)
                f=$OPTARG
                multiHosts=1
                ;;
             t)
                t=$OPTARG
                ;;
             e)
                e=$OPTARG
                graph=1
                ;;
             ?) 
            echo "unkonw argument"
        exit 1
        ;;
        esac
done



# Create graph
if [ $graph -eq 1 ]
then
    ./GRAPH $e
else
    if [ $multiHosts -eq 1 ]
    then
            # multi hosts
            mpiexec -n $n -f $f ./MPI_OMP_PR $t
    else
            # single host
            ./OMP_PR $t
    fi
fi




