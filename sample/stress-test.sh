#!/bin/bash

{
for i in $(seq 0 10) ; do
	echo "Dennis Ritchie" 
done
} > "in.txt"

start_time=$SECONDS

./client-good 0.0.0.0 12343 < "in.txt" > "out.txt"

end_time=$SECONDS

elapsed_time=$(( end_time - start_time ))
echo "Elapsed time: $elapsed_time seconds"
