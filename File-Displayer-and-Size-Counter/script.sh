#!/bin/bash
suffix=$1
n=$2

find . -name '*.'$suffix -printf "%p %s\n" | sort -k2 -n -r | head -$n 
find . -name '*.'$suffix -printf "%p %s\n" | sort -k2 -n -r | head -$n | awk '{sum+=$2;} END {print sum;}'

