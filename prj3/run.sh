
#!/bin/bash


while getopts "j:i:o:" arg 
do
        case $arg in
             j)
                j=$OPTARG
                ;;
             i)
                i=$OPTARG
                ;;
             o)
                o=$OPTARG
                ;;
             ?) 
            echo "unkonw argument"
        exit 1
        ;;
        esac
done


hadoop jar $j -input $i -output $o -mapper 'python3 mapper.py' -reducer 'python3 reducer.py'
 