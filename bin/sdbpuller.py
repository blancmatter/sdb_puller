import os
import datetime

def test():
    print("Sdbpuller module ok")

def getFileList(path):
    files = []
    # r=root, d=directories, f = files
    for r, d, f in os.walk(path):
        for file in f:
            if '.sdb.gz' in file:
                files.append(os.path.join(r, file))
    return files


def pruneFileList(filelist, year):
    now = datetime.datetime.now()
    files = []
    for f in filelist:
        if int(f[-15:-13]) > year && int(f[-15:-13]) <= now.ToString("yy"):
            files.append(f)
    return files
