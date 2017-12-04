#!bash

echo "homework test"

./subscriber 1 &
sleep 0.2s
./publisher 1 &
./publisher 2 

# ./publisher 3
