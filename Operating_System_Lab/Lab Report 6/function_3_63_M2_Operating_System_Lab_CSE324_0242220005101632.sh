#!/bin/bash

sum(){
    total=0
    local total1=0

    for i in $@; do
       (( total += i ))
       (( total1 += i ))
    done

    return $total1
}

multiplicationTable(){
    local num=$1
    for i in {1..10}; do
        echo "$num * $i = $(($num * $i))"
    done
}

total=20
echo "Before function call, total = $total."

num=(10 20 30 40 50)
sum ${num[@]}
echo "Total summation is: $?"

sum 50 60 70
value=$?

echo "After function call, total = $total."
echo -e "Value = $value.\n"

read -p "Enter a number: " num
multiplicationTable $num