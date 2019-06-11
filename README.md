# sdb puller

System to extract data from sdb files from the Liverpool Telescope and format them for ingestion into influx DB, which can be queried with grafana.
As the Std util is compiled for a 32-bit system, and attempts to recompile for 64-bit have been unsuccessful, this has to be run using a run using a 32-bit VM, for which the Vagrantfile is provided here.

## Getting Started



### Prerequisites

* Python 3 with the following modules (many standard);
  * configparser
  * import csv
  * import datetime
  * import glob, os
  * import re
* An installation of InfluxDB with influx CLI tool
* VirtualBox
* Vagrant
* Vagrant box ubuntu/trusty32



In ubuntu these virtual machine dependencies can be met with;

```
sudo apt-get install virtualbox
sudo apt-get install vagrant
vagrant box add ubuntu/trusty32
```

No further provisioning of the system should be required.

### Starting up and running

To start the virtual machine, just run the command

```
vagrant up
```

and then connect using ssh;

```
vagrant ssh
```

The Vagrant file mounts the local directory into the /ttl/sw directory, which is hardcoded and built into the sdb utilities.

The local directory also is mounted in /sdb_puller,

On Ubuntu systems, the VM can be started automatically on boot by adding the following to the /etc/rc.local file, stating myuser as the user that created the virtual machine. In the implementation on the system this is the eng user.

```
cd /sdb_puller/ && su -c "/usr/bin/vagrant up" myuser
exit 0
```

## Getting Started

The key files are;

* [bin/influx_migrate.sh](bin/influx_migrate.sh) - Shell script to go through the sdb files, modify the migrate.cfg template and run the Std extraction from inside the 32bit VM.
* [bin/influx_parse.py](bin/influx_parse.py) - parsing file to create an influx_DB ingestion file.
* [Lists of Datums and Config files](conf/datums/) - Lists of the datums which will be searched, split into groups of 80
* [Datum file and config file creation scripts](conf/scripts/) - Shell scripts for creating the datums.

## Work to do




## Authors


* **Doug Arnold** - *Initial work*
* **Martina Zubac** - *Setup and Testing of Grafana*
* **Marco Lam** - *DB setup and support*
