import sdbpuller as sp
import inotify.adapters
import sys


def main():
    dir = sys.argv[1]
    i = inotify.adapters.InotifyTree(dir)

    for event in i.event_gen(yield_nones=False):
        (_, type_names, path, filename) = event

        if 'IN_CREATE' in type_names:
            file = path + "/" + filename

            if not sp.fileExists(file):
                sdb = sp.sdbFile(file)
                sdb.primeScratch()
                sdb.callStd()
                sdb.sdbParse()
                sdb.importFlx()
                sdb.cleanUp()
                del sdb
            else:
                print ("File Exists")


if __name__ == '__main__':
    main()
