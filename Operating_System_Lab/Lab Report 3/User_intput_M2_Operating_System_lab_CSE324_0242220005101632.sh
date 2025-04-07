#!/bin/bash

# Single input
read -p "Enter your name: " name
echo "Hello, $name."

# Multiple input
read -p "Enter your first and last name: " fast last
echo "Your fast name is $fast and last name is $last"

# Slient Mode -s for sensitive input:
read -sp "Enter your password: " pass
echo -e "\nYour password is: $pass."

# -n character limit
read -n 5 -p "Enter a 5-character code: " code
echo -e "\nYour code is: $code"

# -t wait only a set number of seconds for input
read -t 5 -p "Enter something within 5 sec: " res
echo "Your entered: $res"