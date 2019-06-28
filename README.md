# sdb puller

System to extract data from sdb files from the Liverpool Telescope and format them for ingestion into influx DB, which can be queried with grafana.
As the Std util is compiled for a 32-bit system, and attempts to recompile for 64-bit have been unsuccessful, this has to be run using a run using a 32-bit VM, for which the Vagrantfile is provided here.

In order to speed up ingestion, work is taking place on direct extraction without using the 32-bit Std tool from TTL. A Java class was provided by Steve Foale at LCO, which provides a functional template in [sdb-lib/](sdb-lib/) folder to acheive this.

## Implementation and Usage on sdbinflux server

As of 18th June 2019, the sdbpuller is currently deployed on a test server (HP Proliant DL160). This NFS mounts **/sdb/LT/sdb/** as read only from sdbserver to **/sdb/** locally.

### importhour.py usage

an hour or several hours of data can be imported to the influx database using importhour.py. **Always** pass absolute path to importhour.py

For example;

* Import all data from 20121118;
```shell
/sdb_puller/bin$ ./importhour.py /sdb/2012/201211/121118*.sdb.gz
```


### Daemon Usage

The daemon is called with options **start|stop|restart|debug**. Debug runs the daemon without fully daemonizing, so terminal stdout and stderr can be viewed.

To start, simply type;
```shell
 sdb_puller/bin/importd.py start
```

This will create a pid lock file with the daemon process pid inside and redirect all output to /dev/null.

The daemon operates as follows;
* Every 2 minutes check for the latest modifed file in the NFS mounted /sdb directory from sdbserver
* If that file has not been imported:
  * Wait 30 seconds, to ensure file is fully written (if in case of just arriving from site)
  * Create an instance of the sdbFile class for that hour of data.
  * Perform necessary methods to import the hour of data.

```py
while True:
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
          time.sleep(120)
```





## Getting Started

### Prerequisites

* Python 3 with the following modules (many standard);
  * import configparser
  * import csv
  * import datetime
  * import glob, os
  * import re
  * import influxdb
* An installation of InfluxDB with influx CLI tool
* VirtualBox
* Vagrant
* Vagrant box ubuntu/trusty32 (should load automatically on vagrant up)


In ubuntu the virtual machine dependencies can be met with;

```
sudo apt-get install virtualbox
sudo apt-get install vagrant
vagrant box add ubuntu/trusty32
```

No further provisioning of the system should be required.

### Starting up and running

Place the repsoitory in the root filesystem (i.e. /sdb_puller/). A number of system variables and hard scripted elements require this.

To start the virtual machine.

```sh
cd /sdb_puller/
vagrant up
```

The Vagrant file mounts the local directory into the /ttl/sw directory, which is hardcoded and built into the sdb utilities.

The local directory also is mounted in /sdb_puller,

On Ubuntu systems, the VM can be started automatically on boot by adding the following to the /etc/rc.local file, stating myuser as the user that created the virtual machine. In the implementation on the system this is the eng user.

```sh
cd /sdb_puller/ && su -c "/usr/bin/vagrant up" myuser
exit 0
```

## Key files

The key files are;
* [bin/importd.py](bin/importd.py) -

* [bin/sdbpuller.py](bin/sdbpuller.py) - Contains sdbFile class with methods to prep scratch directories, parse csv output into influx importable ASCII files and call the influx import.

* [bin/sdbpuller.ini](bin/sdbpuller.ini) - Config file with some path options used in sdbpuller.py and others.

* [bin/callStd.sh](bin/callStd.sh) - Called with an hour value. Script to copy and parse config files and run Std tool for each file. Then places csv output into ouptput directory.

* [conf/makedatums/makedatums.sh](conf/makedatums/makedatums.sh) - script to be fed a list of Sources and Datums and split into Std config files of n datums each.

* [conf/datums/](conf/datums/) - Lists of the datums which will be searched, currently split into groups of 80

* [importhour.py](bin/importhour.py) - Example script creating instance of sdbFile and performing tasks to import data into influx. Is to be called with absolute path to sdb.gz files. Can be given multiple file arguments, including wildcard usage.

## Work to do
- [ ] Speed up ingestion via direct access to sdb file reading using sdb-lib (java classes written by Martin Norbury) to get datum values direct from an sdb file.
- [x] Perform error checking on extraction and import to influx
- [x] Create logging to monitor for errors
- [x] Create log of hours successfully ingested
- [x] Provide a deamon version / cron callable element, which will import any new files which come from the telescope on the NFS mounted /sdb/

For a list of cuurent enhancements and bugs, see the [repository issues page](/issues/)






## Authors

* **Doug Arnold** - *Initial work*
* **Martina Zubac** - *Setup and Testing of Grafana*
* **Marco Lam** - *DB setup and support*
