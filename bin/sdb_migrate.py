#!/usr/bin/env python3

import sdbpuller

"""
python script to migrate existing sdbdata
into an influx or mysql databases
"""










def getFileList ():


def
IN_FILE = sys.argv[1]

with open(IN_FILE) as csvfile:
    inputfile = csv.reader(csvfile, delimiter=',')
    data = []
    for row in inputfile:
        data.append(row)
