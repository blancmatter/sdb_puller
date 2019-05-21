import os

def getFilelist(path):
    files = []
    for r, d, f in os.walk(path):
        if '.sdb.gz' in file:
            files.append(os.path.join(r,file))
    return files
