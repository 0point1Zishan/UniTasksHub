#!/bin/bash

i=1
while(( i <= 10 ));do
        echo $i
        (( i++ ))
done

echo ""
password=123
echo -n "Enter password: "
read pass
while((pass != password));do
        echo -n "Wrong password! Enter password agin: "
        read pass
done
echo "System opend!"
echo