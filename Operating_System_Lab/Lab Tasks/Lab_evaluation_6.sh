#!/bin/bash

# Circle area function
circle(){
    local r=$1
    local pi=3.1416
    local area=$(echo "scale=2; $pi * $r * $r" | bc)
    echo "$area"
}

# Rectangle area function
rectangle_area(){
        local length=$1
        local width=$2
        echo "scale=2; $length * $width" | bc
}

# Triangle area function
triangle_area(){
        local base=$1
        local height=$2
        echo "scale=2; 0.5 * ($base * $height)" | bc
}

circle_area=$(circle 5)
echo "Circle's total area = $circle_area."

rec_area=$(rectangle_area 5 10)
echo "Rectangle's total area = $rec_area."

tri_area=$(triangle_area 4 9)
echo "Triangle's total area = $tri_area."