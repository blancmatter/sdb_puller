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
    # Convert from seconds to nanoseconds
    data[i][2] = float(data[i][2]) * 100000000


lines = []
for i in range(len(data)):
    s = "{} ".format(TABLE_NAME)
    if data[i][3] != '' :
        s += "AZR.AXIS_POSITION={},".format(data[i][3])
    if data[i][4] != '' :
        s += "ELR.AXIS_POSITION={},".format(data[i][4])
    if data[i][5] != '' :
        s += "CSR.AXIS_POSITION={},".format(data[i][5])
    if data[i][6] != '' :
        s += "AZR.AXIS_ENC_1_COUNT={},".format(data[i][6])
    if data[i][7] != '' :
        s += "AZR.AXIS_ENC_2_COUNT={},".format(data[i][7])
    if data[i][8] != '' :
        s += "AZR.AXIS_ENC_3_COUNT={},".format(data[i][8])
    if data[i][9] != '' :
        s += "AZR.AXIS_ENC_4_COUNT={},".format(data[i][9])
    if data[i][10] != '' :
        s += "ELR.AXIS_ENC_1_COUNT={},".format(data[i][10])
    if data[i][11] != '' :
        s += "ELR.AXIS_ENC_2_COUNT={},".format(data[i][11])
    if data[i][12] != '' :
        s += "TCS.M1_TILT_MIRROR_XI={},".format(data[i][12])
    if data[i][13] != '' :
        s += "TCS.M1_TILT_MIRROR_ETA={},".format(data[i][13])
    if data[i][14] != '' :
        s += "TCS.M1_SHIFT_FOCAL_PLANE_XI={},".format(data[i][14])
    if data[i][15] != '' :
        s += "TCS.M1_SHIFT_FOCAL_PLANE_ETA={},".format(data[i][15])
    if data[i][16] != '' :
        s += "TCS.M2_TILT_MIRROR_XI={},".format(data[i][16])
    if data[i][17] != '' :
        s += "TCS.M2_TILT_MIRROR_ETA={},".format(data[i][17])
    if data[i][18] != '' :
        s += "TCS.M2_SHIFT_FOCAL_PLANE_XI={},".format(data[i][18])
    if data[i][19] != '' :
        s += "TCS.M2_SHIFT_FOCAL_PLANE_ETA={},".format(data[i][19])
    if data[i][20] != '' :
        s += "MSR.RED_AXIAL_VALVE_FBK={},".format(data[i][20])
    if data[i][21] != '' :
        s += "MSR.YEL_AXIAL_VALVE_FBK={},".format(data[i][21])
    if data[i][22] != '' :
        s += "MSR.BLU_AXIAL_VALVE_FBK={},".format(data[i][22])
    if data[i][23] != '' :
        s += "MSR.RED_AXIAL_VALVE_DRIVE={},".format(data[i][23])
    if data[i][24] != '' :
        s += "MSR.YEL_AXIAL_VALVE_DRIVE={}".format(data[i][24])
    if data[i][25] != '' :
        s += "MSR.BLU_AXIAL_VALVE_DRIVE={},".format(data[i][25])


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
