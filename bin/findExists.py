#!/usr/bin/env python3

import sdbpuller as sp

f= open("imported.txt","w+")
n = open("nimported.txt","w+")

files = sp.getFileList('/sdb/')
sorted = sp.sortFiles(files)

for file in sorted:
    sdb = sp.sdbFile(file)

    if sdb.testImport():
        print(file, " exists")
        f.write(file)
    else:
        print(file, " does not exist")


f.close
n.close
