#!/bin/bash

multiple_parameter(){
    echo "File name: $0"
    echo "1st parameter: $1"
    echo "2nd parameter: $2"
    echo "3rd parameter: $3"
    echo "4th parameter: $4"
}

arrayParameter(){
    echo "Number of element in the array $#"
    echo "1-st element: $1"
    echo "Elements are: $@"

    for i in $@; do
        echo $i
    done
}

factorial(){
    local n=$1
    if (( n == 1 )); then
        echo 1
    else
        local pre=$(factorial $(( n-1 )) )
        echo $(( n * pre ))
    fi
}

multiple_parameter 10 "abc" 3.8 'a'
echo ""

read -a num -p "Enter some elements: "
arrayParameter ${num[@]}

res=$(factorial 5)
echo "Factorial of 5 is: $res."