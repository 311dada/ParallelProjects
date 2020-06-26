#!/bin/bash


make clean
make

while getopts "n:f:e:" arg 
do
        case $arg in
             n)
                n=$OPTARG
                ;;
             f)
                f=$OPTARG
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


mpiexec -n $n -f $f ./allgather $e