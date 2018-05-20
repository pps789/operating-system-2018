#!/bin/bash
i=1
j=1
num=17942473
for j in 1 4 8 12 16 20
do
    echo `./workload $num 1 $j`
done
for ((i=5;i<101;i+=5));
do
    for j in 1 4 8 12 16 20
    do
        echo `./workload $num $i $j`
    done
done
exit 0

