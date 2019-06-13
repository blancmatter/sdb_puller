#!/usr/bin/env bash

# Create the list files with sets of n datums in
# USAGE ./makeslists listfile num_datums

split -l $2 $1 -d list

for file in list*
do
   echo $file
   number=${file:4:2}
   cat $file | sed 's/ //g' | sed 's/^/DAT,/g' > datums$number.lst

done

rm list*


for file in d*.lst
do
   head -n 13 confTemplate > temp
   cat $file | sed 's/,/, /g'>> temp
   tail -n 3 confTemplate >> temp
   newfile=${file%.*}.cfg
   echo $newfile
   cat temp > $newfile
   rm temp
done

rm -r ../datums/*
mv *.cfg *.lst ../datums/
