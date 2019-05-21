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
        # if the 2 digit year is greater than the prune year AND lesseq than current year
        if int(f[-15:-13]) > year and int(f[-15:-13]) <= now.year % 100:
            files.append(f)
    return files
