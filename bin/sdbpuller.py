import os
import datetime
import configparser

# Locate and read in the config file
fileDir = os.path.dirname(os.path.abspath(__file__))
iniFile = fileDir + '/sdbpuller.ini' # should always be in the same dir as this file.
config = configparser.ConfigParser()
config.read(iniFile)


def test():
    print("Sdbpuller module ok")


def getFileList(path):
    '''
    Return a list of all sdb files in the directory structure
    '''
    files = []
    # r=root, d=directories, f = files
    for r, d, f in os.walk(path):
        for file in f:
            if '.sdb.gz' in file:
                files.append(os.path.join(r, file))
    return files


def pruneFileList(filelist, year):
    '''
    Return a pruned list of files, valid after a certain year
    '''
    now = datetime.datetime.now()
    files = []
    for f in filelist:
        # if the 2 digit year is greater than the prune year AND lesseq than current year
        if int(f[-15:-13]) >= year and int(f[-15:-13]) <= now.year % 100:
            files.append(f)
    return files


class sdbFile:
    '''
    Class for an hour of sdbobservation
    '''

    def __init__(self, path):
        '''
        The times of the file based on the filename string
        '''
        self.path = path
        self.filename = path[-15:]
        self.date = path[-15:-7]
        self.year = path[-15:-13]
        self.month = path[-13:-11]
        self.day = path[-11:-9]
        self.hour = path[-9:-7]


    def createConfigs(self):
        '''
        Create the config files
        '''
