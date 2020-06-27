#!/bin/bash


make clean
make


multiHosts=0
while getopts "n:f:e:" arg 
do
        case $arg in
             n)
                n=$OPTARG
                ;;
             f)
                f=$OPTARG
                multiHosts=1
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
   mpiexec -n $n -f $f ./allgather $e
else
   mpiexec -n $n ./allgather $e
fi
