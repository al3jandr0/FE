#!/bin/bash   

# running using the following: 
# ./testing.sh localhost 50000 200 100
# It will make 200 clients each of which will make 100 random moves.  

if [ $# -lt 4 ] ; then
   		echo "Error!"
   		echo "Usage: $0 <IP> <Port - Number> <Number of clients> <Number of moves per client>"
		exit
	fi

rm ./runner.txt

echo "connect $1 $2" > runner.txt
echo "join"

for (( i=1; i<=$4; i++ ))
do
	number=$RANDOM
	let "number %= 4"

if [ "$number" == "1" ]
then
  echo "move U" >> runner.txt
elif [ "$number" == "2" ] 
then
  echo "move D" >> runner.txt
elif [ "$number" == "3" ] 
then
  echo "move R" >> runner.txt
elif [ "$number" == "4" ] 
then
  echo "move L" >> runner.txt 
fi  


done


while read input
do echo "$input"
   sleep 1
done < runner.txt | ./client/client $1 $2 


#for (( i=1; i<=$3; i++ ))
#do
#	echo $i
#./client $1 $2 < runner.txt
#done
