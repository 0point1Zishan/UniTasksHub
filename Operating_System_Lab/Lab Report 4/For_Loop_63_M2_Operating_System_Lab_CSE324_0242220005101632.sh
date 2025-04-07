#!/bin/bash
read -p "Enter some number: " nums
for i in ${nums[@]}; do
    echo $i
done

echo -e "\nSome odd numbers are: "
for i in {1..10..2}; do
    echo $i
done