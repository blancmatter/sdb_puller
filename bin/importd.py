#!/usr/bin/env python3
import sdbpuller as sp
import sys, time


def _main():
    while True:
        files = sp.getFileList('/sdb')
        mostRecent = sp.returnMostRecent(files)
        if not sp.fileExists(mostRecent):
            time.sleep(30) # make sure any writes have finished to filesystem
            sdb = sp.sdbFile(file)
            sdb.primeScratch()
            sdb.callStd()
            sdb.sdbParse()
            sdb.importFlx()
            sdb.cleanUp()
            del sdb
        else:
            time.sleep(5)



if __name__ == '__main__':
    _main()
