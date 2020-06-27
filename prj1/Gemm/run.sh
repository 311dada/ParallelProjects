
#!/bin/bash


make clean
make


multiHosts=0
conv=0
pooling=0
random=0
matrix=0
while getopts "n:f:mcrM:K:p" arg 
do
        case $arg in
             n)
                n=$OPTARG
                ;;
             f)
                f=$OPTARG
                multiHosts=1
                ;;
             m)
                matrix=1
                ;;
             c)
                conv=1
                ;;
             r)
                random=1
                ;;
             M)
                mfile=$OPTARG
                ;;
             K)
                kfile=$OPTARG
                ;;
             p)
                pooling=1
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
   # matrix multiplication
   if [ $matrix -eq 1 ]
   then
      if [ $random -eq 1 ]
      then
        mpiexec -n $n -f $f ./mm random
      else
        mpiexec -n $n -f $f ./mm
      fi
    
    # convolution
    elif [ $conv -eq 1 ]
    then
       mpiexec -n $n -f $f ./conv $mfile $kfile
    
    # max pooling
    elif [ $pooling -eq 1 ]
    then
       mpiexec -n $n -f $f ./pooling $mfile
    
    else
       echo "Invalid command"
    fi
else
   # single host

   # matrix multiplication
   if [ $matrix -eq 1 ]
   then
      if [ $random -eq 1 ]
      then
        mpiexec -n $n ./mm random
      else
        mpiexec -n $n ./mm
      fi
    
    # convolution
    elif [ $conv -eq 1 ]
    then
       mpiexec -n $n ./conv $mfile $kfile
    
    # max pooling
    elif [ $pooling -eq 1 ]
    then
       mpiexec -n $n ./pooling $mfile
    
    else
       echo "Invalid command"
    fi
fi
