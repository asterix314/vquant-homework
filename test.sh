#!bash

./subscriber 1 &
sleep 0.5s
./publisher 1 &
./publisher 2
