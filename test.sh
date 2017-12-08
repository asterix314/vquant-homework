#! /bin/bash

echo "homework test"

taskset 0x0001 ./subscriber 1 &
# ./subscriber 1 &

# as root 
# taskset 0x00000001 nice -n -10 ./subscriber 1 & 

sleep 0.2s

taskset 0xFFFE ./publisher 1 &
# ./publisher 1 &

taskset 0xFFFE ./publisher 2 &
# ./publisher 2 &

# taskset 0xFFFE ./publisher 3 &
