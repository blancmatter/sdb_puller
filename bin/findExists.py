#!/usr/bin/env python3

import sdbpuller as sp

f= open("imported.txt","w+")
n= open("nimported.txt","w+")

files = sp.getFileList('/sdb/')
sorted = sp.sortFiles(files)

for file in sorted:
    sdb = sp.sdbFile(file)

    if sdb.testImport():
        print(file, " exists")
        print(file, file=f)
    else:
        print(file, " does not exist")
        print(file, file=n)

f.close
n.close
