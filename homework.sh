#!bash

./publisher 1 &
sleep 0.2s
./publisher 2 &
./subscriber 1 &
