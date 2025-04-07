#!/bin/bash
echo -n "Did you complete the lab report? Answer only in [Yes/No]: "
read ans

case $ans in
Yes | yes | *y | *Y) echo "Good Job!";;
No | no) echo "It's okay";;
*) echo "Can't recognized";;
esac