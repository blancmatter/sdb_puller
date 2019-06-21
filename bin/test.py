#!/usr/bin/env python3
import sys
import pdb
import sdbpuller as sp


files = sp.getFileList('/home/aridarno/Desktop')
mostrecent = sp.returnMostRecent(files)

print(mostrecent)
