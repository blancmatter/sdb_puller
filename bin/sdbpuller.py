import os

def test():
    print("Sdbpuller module ok")

def getFileList(path):
    files = []
    # r=root, d=directories, f = files
    for r, d, f in os.walk(path):
        for file in f:
            if '.sdb.gz' in file:
                print(file)
                files.append(os.path.join(r, file))
    return files


def pruneFileList(filelist, year):
    files = []
    for f in filelist:
        if int(f[-15:-13]) > year:
            files.append(f)
    return files
