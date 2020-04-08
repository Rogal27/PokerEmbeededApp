#!/bin/bash

BRPATH=/home/rogal-old/linsw/lab2/test1/LINES_LAB_2_Jan_Kowalski/buildroot-2020.02

(
export PATH=$BRPATH/output/host/usr/bin:$PATH
make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabihf-
)
