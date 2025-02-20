#!/bin/bash

# Create a directory for logs if it doesn't exist
mkdir -p logs

# Function to make a request and log the response time
make_request() {
    local size=$1
    local request_num=$2
    start_time=$(date +%s.%N)
    curl -s "http://localhost:10000/spin.cgi?size=$size" > /dev/null
    end_time=$(date +%s.%N)
    duration=$(echo "$end_time - $start_time" | bc)
    echo "$request_num,$size,$duration" >> logs/response_times.csv
}

# Initialize the log file with headers
echo "request_num,size,duration" > logs/response_times.csv

# Create an array of all requests (1000 each for size=512 and size=1024)
requests=()
for i in $(seq 1 500); do
    requests+=("512")
    requests+=("1024")
done

# Shuffle the array to randomize the order of requests
for i in $(seq ${#requests[@]} -1 1); do
    j=$(($RANDOM % i))
    temp=${requests[$i-1]}
    requests[$i-1]=${requests[$j]}
    requests[$j]=$temp
done

# Make parallel requests
request_num=1
for size in "${requests[@]}"; do
    make_request "$size" "$request_num" &
    request_num=$((request_num + 1))
done

# Wait for all background processes to complete
wait

echo "All requests completed. Results are in logs/response_times.csv"

# Calculate and display summary statistics
echo "\nSummary Statistics:"
echo "For 512KB requests:"
awk -F',' '$2==512 {sum+=$3; count++} END {print "Average response time: " sum/count " seconds"}' logs/response_times.csv
echo "For 1024KB requests:"
awk -F',' '$2==1024 {sum+=$3; count++} END {print "Average response time: " sum/count " seconds"}' logs/response_times.csv