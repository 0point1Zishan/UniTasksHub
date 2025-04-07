#!/bin/bash

add(){
    (( sum += $1 ))
    return $sum
}

sum=0
for i in {10..50..10}; do
    add $i
    echo "Sum = $?"
done

echo "Total Value = $sum."
