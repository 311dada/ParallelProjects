#!/bin/bash

make clean
make

multiHosts=0

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
                ;;
             ?) 
            echo "unkonw argument"
        exit 1
        ;;
        esac
done


if [ $multiHosts -eq 1 ]
then
   # multi hosts
   mpiexec -n $n -f $f ./MPI_OMP_PI $e $t
else
   # single host
   ./OMP_PI $e $t
fi
