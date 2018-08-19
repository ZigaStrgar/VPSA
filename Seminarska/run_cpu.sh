#!/bin/bash
gcc cpe.c -o cpu -O3
for run in {1..10}
do
  ./cpu
done
