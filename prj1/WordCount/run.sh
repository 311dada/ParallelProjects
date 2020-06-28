
#!/bin/bash

make clean
make


multiHosts=0
big=0
small=0
bigFreq=0
smallFreq=0

while getopts "n:f:bsBS" arg 
do
        case $arg in
             n)
                n=$OPTARG
                ;;
             f)
                f=$OPTARG
                multiHosts=1
                ;;
             b)
                big=1
                ;;
             B)
                bigFreq=1
                ;;
             s)
                small=1
                ;;
             S)
                smallFreq=1
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
   
   # big file count
   if [ $big -eq 1 ]
   then
      mpiexec -n $n -f $f ./big
    
    # big frequency 
    elif [ $bigFreq -eq 1 ]
    then
       mpiexec -n $n -f $f ./big_freq
    
    # small file count
    elif [ $small -eq 1 ]
    then
       mpiexec -n $n -f $f ./small
    
    # small frequency
    elif [ $smallFreq -eq 1 ]
    then
       mpiexec -n $n -f $f ./small_freq
    
    else
       echo "Invalid command"
    fi
else
   # single host

   # big file count
   if [ $big -eq 1 ]
   then
      mpiexec -n $n ./big
    
    # big frequency 
    elif [ $bigFreq -eq 1 ]
    then
       mpiexec -n $n ./big_freq
    
    # small file count
    elif [ $small -eq 1 ]
    then
       mpiexec -n $n ./small
    
    # small frequency
    elif [ $smallFreq -eq 1 ]
    then
       mpiexec -n $n ./small_freq
    
    else
       echo "Invalid command"
    fi
fi
