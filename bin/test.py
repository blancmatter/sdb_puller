#!/usr/bin/env python3
import sys
import pdb
import sdbpuller as sp


if not sp.fileExists('/sdb/2019/06/18040506.sdb.gz'):
    print ("Run Code")
else:
    print ("File Exists")


#sdb = sp.sdbFile('/sdb/19050602.sdb.gz')
#pdb.set_trace()
#sdb.primeScratch()
#sdb.callStd()
#sdb.sdbParse()
#sdb.importFlx()
#sdb.cleanup()
