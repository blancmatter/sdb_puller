#!/usr/bin/env python3
import sdbpuller as sp
import sys, time


def _main():
    """
    daemon to run on sdbinflux
    """

    # Deamonize process
    sp.daemonize()


    while True:
        files = sp.getFileList('/sdb')
        mostRecent = sp.returnMostRecent(files))
        print(mostRecent)
        if not sp.fileExists(mostRecent):
            time.sleep(30) # make sure any writes have finished to filesystem
            sdb = sp.sdbFile(mostRecent)
            sdb.primeScratch()
            sdb.callStd()
            sdb.sdbParse()
            sdb.importFlx()
            sdb.cleanUp()
            del sdb
        else:
            time.sleep(120)



if __name__ == '__main__':
    _main()
