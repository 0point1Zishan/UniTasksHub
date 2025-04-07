#!/bin/bash
read -p "Enter your age: " age
if (( age >= 18 )) then
    echo "You are Adult"
else 
    echo "You are child."
fi