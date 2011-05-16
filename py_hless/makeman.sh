#!/bin/bash

txt2man -P hless -p -s 1 -v 'Alexander Sandler' \
-I'infile' \
-B'Search Patterns Menu' \
-B'show patterns' \
hless.txt | sed 's/\\/\\\\/g' > tmp1
grep -B 100000 "MAN STARTS HERE" hless > tmp2
grep -A 100000 "MAN ENDS HERE" hless > tmp3

cat tmp2 > hless
cat tmp1 >> hless
cat tmp3 >> hless
rm -f tmp1 tmp2 tmp3

