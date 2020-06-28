#!/bin/bash

make clean
make

multiHosts=0
display=0

while getopts "n:f:t:e:m:d" arg 
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
             m)
                m=$OPTARG
                ;;
             d)
                display=1
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
   if [ $display -eq 1 ]
   then
        mpiexec -n $n -f $f ./MPI_OMP_QC $e $t $m display
   else
        mpiexec -n $n -f $f ./MPI_OMP_QC $e $t $m
   fi
else
   # single host
   if [ $display -eq 1 ]
   then
        ./OMP_QC $e $t $m display
   else
        ./OMP_QC $e $t $m
   fi
fi

