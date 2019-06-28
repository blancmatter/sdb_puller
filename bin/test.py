#!/usr/bin/env python3
import sys, time
import pdb
import sdbpuller as sp


import parset as ps

file = ps.sdbFile(sys.argv[1])
print (file.year)
