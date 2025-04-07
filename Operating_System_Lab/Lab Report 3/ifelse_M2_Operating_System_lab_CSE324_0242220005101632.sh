#!/bin/bash
read -p "Enter your age: " age
if (( age >= 18 )) then
    echo "You are an Adult."
else 
    echo "You are a child."
fi