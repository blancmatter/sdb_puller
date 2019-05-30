#!/usr/bin/env python3
import sys
import sdbpuller as sp

file = sys.argv[1]

sdb = sp.sdbFile(file)
sdb.primeScratch()
sdb.callStd()
sdb.sdbParse()
sdb.importFlx()
sdb.cleanup()
