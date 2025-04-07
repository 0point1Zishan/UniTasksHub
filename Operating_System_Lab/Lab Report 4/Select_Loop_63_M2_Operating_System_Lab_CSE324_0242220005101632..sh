#!/bin/bash
num=(1 2 3 4 5)
echo "Select a number: "


select i in ${num[@]}; do
    if [ -n "$i" ]; then
        echo "You selected: $i"
        break 
    else
        echo "Invalid Selection."
    fi
done