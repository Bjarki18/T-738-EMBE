#!/bin/bash
#  Short script to toggle a GPIO pin at the highest frequency
#  possible using Bash - by Derek Molloy
echo 20 > /sys/class/gpio/export
sleep 0.5
echo 17 > /sys/class/gpio/export


echo "in" > /sys/class/gpio/gpio20/direction
echo "out" > /sys/class/gpio/gpio17/direction
COUNTER=0
while [ $COUNTER -lt 100000 ]; do
    cat /sys/class/gpio/gpio20/value | sudo tee /sys/class/gpio/gpio17/value
    let COUNTER=COUNTER+1
done

echo 17 > /sys/class/gpio/unexport
echo 20 > /sys/class/gpio/unexport
