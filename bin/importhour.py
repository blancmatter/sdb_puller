#!/usr/bin/env python3
import sys
import sdbpuller as sp


files = sys.argv[1:]

for file in files:
    sdb = sp.sdbFile(file)
    sdb.primeScratch()
    sdb.callStd()
    sdb.sdbParse()
    sdb.importFlx()
    sdb.cleanUp()
