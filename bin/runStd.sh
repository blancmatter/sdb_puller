#!/usr/bin/env bash
# Script to be called from the Virtual machine to run Std on the files from the sdb_puller
#
# Version 0.1 Doug Arnold 24/05/19
#

# Inputs
YEAR = $1
MONTH = $2
DAY = $3
HOUR = $4


# Sed the config files to update the times, dates and output




# Call Std for each .cfg file in the directory and convert to a
# CSV format, so that python can modify for influx ingestion
cd /sdb_puller/sdbscratch
/ttl/sw/util/Std -conf test.cfg -path ./
