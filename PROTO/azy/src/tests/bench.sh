#!/bin/sh

for ((i=0;i<100;i++)); do

  ./client &

done

wait
