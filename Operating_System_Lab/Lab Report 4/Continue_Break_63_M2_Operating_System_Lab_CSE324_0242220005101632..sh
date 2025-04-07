#!/bin/bash

# Continue
echo "Odd numbers: "
for i in {1..10}; do
    if (( i % 2 == 0 )); then
        continue
    else
        echo $i
    fi 
done

# Break
echo -e "\nNatural numbers are:"
for i in {1..10}; do
    if (( i > 5 )); then
        break 
    fi
    echo $i
done