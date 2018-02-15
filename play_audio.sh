#!/usr/bin/env bash

for i in *.raw; do
    play -b 16 -e signed -r 22050 $i
done;
