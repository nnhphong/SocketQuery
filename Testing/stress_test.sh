#!/bin/bash

{
for i in $(seq 0 9999) ; do
	echo "Dennis Ritchie" 
done
} > "in.txt"

start_time=$(date +%s)

./client 0.0.0.0 54323 < "in.txt" > "out.txt"

end_time=$(date +%s)

elapsed_time=$(( end_time - start_time ))
echo "Elapsed time: $elapsed_time seconds"
