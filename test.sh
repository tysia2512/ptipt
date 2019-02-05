#!/bin/bash
input=`cat corr.in`
out_file=corr_run.out
touch $out_file
: > $out_file
g++ -g -O2 -std=c++11 -pthread -march=native aks.cpp -o aks -lntl -lgmp -lm
for line in $input;
do
    ./aks <<< $line >> $out_file
done
diff $out_file corr.out
