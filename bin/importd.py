#!/usr/bin/env python3
import configparser
from daemon import Daemon
import sdbpuller as sp
import sys, os, time


# Locate and read in the config file
fileDir = os.path.dirname(os.path.abspath(__file__))
iniFile = fileDir + '/sdbpuller.ini' # should always be in the same dir as this file.
config = configparser.ConfigParser()
config.read(iniFile)

#set the daemon pid file path
pidfile = config['DEFAULT']['dpiddir'] + "importd.pid"


class MyDaemon(Daemon):
    def run(self):
        """
        This is the daemonised process block
        """
        while True:
            """
            files = sp.getFileList(config['DEFAULT']['sdbdir'])
            mostRecent = sp.returnMostRecent(files)
            print(mostRecent)
            if not sp.fileExists(mostRecent):
                # make sure any writes have finished to filesystem
                time.sleep(30)
                sdb = sp.sdbFile(mostRecent)
                sdb.primeScratch()
                sdb.callStd()
                sdb.sdbParse()
                sdb.importFlx()
                sdb.cleanUp()
                del sdb
            else:
                # wait 2 mins before checking again
            """
            time.sleep(120)


if __name__ == "__main__":
    daemon = MyDaemon(pidfile)
    if len(sys.argv) == 2:
        if 'start' == sys.argv[1]:
            daemon.start()
        elif 'stop' == sys.argv[1]:
            daemon.stop()
        elif 'restart' == sys.argv[1]:
            daemon.restart()
        elif 'debug' == sys.argv[1]:
            daemon.debug()

        else:
            print("Unknown command")
            sys.exit(2)
        sys.exit(0)
    else:
            print ("usage: %s start|stop|restart|debug" % sys.argv[0])
            sys.exit(2)
