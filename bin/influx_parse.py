#!/usr/bin/env python

import csv
import sys

IN_FILE = sys.argv[1]
OUT_FILE = sys.argv[2]

TABLE_NAME = sys.argv[3]

print ('influx_parse.py: Filename is ', IN_FILE)

with open(IN_FILE) as csvfile:
    inputfile = csv.reader(csvfile, delimiter=',')
    data = []
    for row in inputfile:
        data.append(row)
    # Remove first line with headers
    data = data[1:]

# Set time in s to microseconds for ingestion into INFLUX_DB
for i in range (len(data)):
    # Convert from seconds to microseconds
    data[i][2] = float(data[i][2]) * 1000000


lines = []
for i in range(len(data)):
    s = "{} ".format(TABLE_NAME)
    if data[i][3] != '' :
        s += "AZR.POSITION={},".format(data[i][3])
    if data[i][4] != '' :
        s += "ELR.POSITION={},".format(data[i][4])
    if data[i][5] != '' :
        s += "CSR.POSITION={},".format(data[i][5])


    # Remove any trailing comma
    if s[-1] == ',' :
        s = s[:-1]
    s+= " {}".format(data[i][2])
    lines.append(s)

file = open(OUT_FILE, 'w+')

file.write("# DDL\n")
file.write("CREATE DATABASE WUMa\n")
file.write("\n")
file.write("# DML\n")
file.write("# CONTEXT-DATABASE: WUMa\n")
file.write("\n")
for item in lines:
    file.write("%s\n" % item)
