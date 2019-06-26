#!/usr/bin/env python3
import sys, time
import pdb
import sdbpuller as sp


sp.daemonize()


while true:
    time.sleep(120)

"""

files = sp.getFileList('/home/aridarno/Desktop')
mostrecent = sp.returnMostRecent(files)

print(mostrecent)
"""
