#!/usr/bin/env bash
# Script to be called from the Virtual machine to run Std on the files from the sdb_puller
#
# Version 0.1 Doug Arnold 24/05/19
#

# Inputs
YEAR=$1
MONTH=$2
DAY=$3
HOUR=$4
HOUR1=$5

dir=/sdb_puller/sdbscratch/$YEAR$MONTH$DAY$HOUR



# Call Std for each .cfg file in the directory and convert to a
# CSV format, so that python can modify for influx ingestion
cd  $dir
rm *.cfg
rm *.dat


FILES=/sdb_puller/conf/datums/d*.cfg

for f in $FILES
do
lf=$(basename $f) # strip path from filename for local file.
confNo=$(echo $f | sed -e s/[^0-9]//g) # take out the conf number
echo $lf $confNo
cat $f | sed "s/M_DATE/$YEAR\/$MONTH\/$DAY/g" | sed "s/M_HOUR/$HOUR/g" | sed "s/M_PLUS_HOUR/$HOUR1/g" | sed "s/M_OUTPUT/$confNo/g" > $lf

/ttl/sw/util/Std -conf $lf -path ./
cat $confNo.dat | sed 's/\t/,/g' | cut -d "," -f 3- | sed '1d'> /sdb_puller/sdboutput/$YEAR$MONTH$DAY$HOUR/$confNo.csv

done
