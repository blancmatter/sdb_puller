import configparser
import csv
import datetime
import glob
import os
import re
from influxdb import InfluxDBClient




# Locate and read in the config file
fileDir = os.path.dirname(os.path.abspath(__file__))
iniFile = fileDir + '/sdbpuller.ini' # should always be in the same dir as this file.
config = configparser.ConfigParser()
config.read(iniFile)



def fileExists(path):
    """
    Check if a filename exists in the import log
    Exists return True
    !Exists return False
    """
    file = path[-15:]

    with open(config['DEFAULT']['logfile']) as f:
        datafile = f.readlines()

    for line in datafile:
        if file in line:
            return True
    return False

def getFileList(path):
    """
    Return a list of all sdb files in the directory structure
    """
    files = []
    sdbRe = re.compile("\d\d\d\d\d\d\d\d.sdb.gz$")

    # r=root, d=directories, f = files
    for r, d, f in os.walk(path):
        for file in f:

            if sdbRe.match(file):
                files.append(os.path.join(r, file))
    return files


def pruneFileList(filelist, year):
    """
    Return a pruned list of files, valid after a certain year
    """
    now = datetime.datetime.now()
    files = []
    for f in filelist:
        # if the 2 digit year is greater than the prune year AND lesseq than current year
        if int(f[-15:-13]) >= year and int(f[-15:-13]) <= now.year % 100:
            files.append(f)
    return files


def sortFiles(paths):
    """
    Return List of files sorted by getctime
    """
    paths.sort(key=lambda x: os.path.getmtime(x), reverse=True)
    return paths




def returnMostRecent(paths):
    """
    Return the most recent file in the files list
    """
    return max(paths, key=os.path.getctime)


class sdbFile:
    """
    Class for an hour of sdbobservation
    """

    def __init__(self, path):
        """
        The times of the file based on the filename string
        """
        self.path = path
        self.filename = path[-15:]
        self.date = path[-15:-7]
        self.year = path[-15:-13]
        self.month = path[-13:-11]
        self.day = path[-11:-9]
        self.hour = path[-9:-7]
        self.hour1 = str(int(self.hour) + 1)
        if len(self.hour1) == 1: # If value is a single digit
            self.hour1 = "0" + self.hour1 # Add a zero in frott


    def callStd(self):
        command = "cd /sdb_puller/ && vagrant ssh -c '/sdb_puller/bin/runStd.sh " + self.year + " " + self.month + " " + self.day + " " + self.hour + " " + self.hour1 + " > " + config['DEFAULT']['logdir'] + self.date + "Std.log'"
        print(command)
        os.system(command)

    def cleanUp(self):
        os.chdir('/')
        command = "rm -rf " + config['DEFAULT']['outputdir'] + "/" + self.date
        print(command)
        os.system(command)
        command = "rm -rf " + config['DEFAULT']['scratchdir'] + "/" + self.date
        print(command)
        os.system(command)

    def createErrorLog(self):
        """
        tar up the scratch and output dirs into the logdir
        will allow further analysis of what went wrong!
        """
        command = "tar -cvf " + config['DEFAULT']['logdir'] + self.date + "_error.tar " + config['DEFAULT']['outputdir'] + " " + config['DEFAULT']['scratchdir']



    def importFlx(self):
        dir = config['DEFAULT']['outputdir'] + "/" + self.date
        os.chdir(dir)
        files = glob.glob("*.flx")
        files.sort()

        for file in files:
            command = "influx -import -path=" + file + " > " + config['DEFAULT']['logdir'] + self.date + "Std.log'"
            print(command)
            os.system(command)

        if self.testImport():
            command = "echo " + self.filename + " >> " + config['DEFAULT']['logfile']
            os.system(command)
        else:
            self.createErrorLog()



    def primeScratch(self):
        """
        Create scratchdir
        Create outputdir
        Prime the scratchdir with the sdb file
        """
        command = "mkdir " + config['DEFAULT']['scratchdir'] + "/" + self.date
        print(command)
        os.system(command)

        command = "mkdir " + config['DEFAULT']['outputdir'] + "/" + self.date
        print(command)
        os.system(command)

        command = "cp " + self.path + " " + config['DEFAULT']['scratchdir'] + "/" + self.date
        print(command)
        os.system(command)



    def sdbParse(self):
        """
        Parse the CSV output file of Std, once run through the VM (see bin/runStd.sh)
        Some CSV files will be empty (no datums values during that hour) and are
        removed before conversion to FLX files.
        """

        dir = config['DEFAULT']['outputdir'] + "/" + self.date
        os.chdir(dir)

        # Remove empty csv files
        command = "find . -size 0 -delete"
        os.system(command)

        files = glob.glob("*.csv")
        files.sort()
        for file in files:
            fileNo = re.findall('\d+', file)
            outFile = fileNo[0] + ".flx"
            print(outFile)
            datFile = "/sdb_puller/conf/datums/datums" + fileNo[0] + ".lst"
            print (datFile)


            with open(file) as csvfile:
                inputfile = csv.reader(csvfile, delimiter=',')
                global data
                data = []
                for row in inputfile:
                    data.append(row)

            # Set time in s to nanoseconds for ingestion into INFLUX_DB
            for i in range (len(data)):
                # Convert from seconds to nanoseconds
                data[i][0] = int(float(data[i][0]) * 1000000000)


            with open(datFile) as csvfile:
                inputfile = csv.reader(csvfile, delimiter=',')
                global datums
                datums = []
                for row in inputfile:
                    datums.append(row)


            lines = [] # setup ingestion lines
            for i in range(len(data)):
                s = "{} ".format("sdbfull")
                for j in range(1, len(datums)+1):
                    if data[i][j]:
                        s += datums[j-1][1] + "." + datums[j-1][2] + "={},".format(data[i][j])

                if s[-1] == ',' :
                    s = s[:-1]
                s+= " {}".format(data[i][0])
                #print(s)
                lines.append(s)


            file = open(outFile, 'w+')

            file.write("# DDL\n")
            file.write("CREATE DATABASE sdbfull\n")
            file.write("\n")
            file.write("# DML\n")
            file.write("# CONTEXT-DATABASE: sdbfull\n")
            file.write("\n")
            for item in lines:
                file.write("%s\n" % item)



    def testImport(self):
        """
        Test that data exists for the hour of imports.
        This is done using the influxDB python client
        - Connect
        - Run a count query which returns the numbers of datums.
        """

        client = InfluxDBClient(host='localhost', port=8086)

        # Create a partial datestring as YY-MM-DD HH:
        dateStr = self.year  + "-" \
                + self.month + "-" \
                + self.day   + " " \
                + self.hour  + ":"

        # Create query using partial datestring. Returns the count of weather reading taking out first and last 30 seconds of the hour.
        queryStr = "SELECT count(*) from \"sdbfull\".\"autogen\".\"sdbfull\" WHERE time > \'20" + dateStr + "00:30\' and time < \'20" + dateStr + "59:30\'"

        # result set returned. Will be empty if no data.
        rs = client.query(queryStr)

        # if data exists
        if rs:
            return True
        else:
            return False
