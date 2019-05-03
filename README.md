# sdb puller

System to extract data from sdb files from the Liverpool Telescope and format them for ingestion into influx DB.
As the Std util is compiled for a 32-bit system, this should be run using a 32-bit VM.

## Getting Started



### Prerequisites

* VirtualBox
* Vagrant
* Vagrant box ubuntu/trusty32

in ubuntu these dependencies can be met with;

```
sudo apt-get install virtualbox
sudo apt-get install vagrant
vagrant box add ubuntu/trusty32
```

No further provisioning of the system should be required. (To be decided once we interface it with influx_DB ingestion)

### Starting up and running

To start the virtual machine, just run the command

```
vagrant up
```

and then connect using shh;

```
vagrant ssh
```

The Vagrant file mounts the local directory into the /ttl/sw directory, which is hardcoded and build into the sdb utilities.

The local directory also is mounted in /vagrant, which can be used to store the sdb files which need to be processed.

Run on a local directory with sdb.gz files in it;
```
/ttl/sw/bin/influx_migrate.sh
```


## Getting Started

The key files are;

* [bin/influx_migrate.sh](bin/influx_migrate.sh) - Shell script to go through the sdb files, modify the migrate.cfg template and run the Std extraction.
* [bin/influx_parse.py](bin/influx_parse.py) - parsing file to create an influx_DB ingestion file with all the data
* [etc/migrate.cfg](etc/migrate.cfg) - template of the DATUM's to be extracted from the sdb server.

## Work to do

- [ ] Create a single config file with sdb DATUM's and their name in the influx_DB. At present this has only been tested with 'hardcoded' elements in [influxparse.py](bin/influxparse.py)
- [ ] Test pulling out all DATUM information from the sdb files (although this means creating 400+ manual lines of entry!)
- [ ] Test and automate the ingestion into INFLUX_DB
- [ ] Deploy to sdbserver and ingest into a local or remote INFLUX_DB


## Authors

* **Martina Zubac** - *Extention and testing*
* **Doug Arnold** - *Initial work*
* **Marco Lam** - *DB setup and support**
