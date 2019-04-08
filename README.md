# sdb puller

System to extract data from sdb files from the Liverpool Telescope and format them for ingestion into influx DB.
As the Std util is compiled for a 32-bit system, this should be run using a 32-bit VM.

## Getting Started



### Prerequisites

* VirtualBox
* Vagrant
* Vagrat box ubuntu/trusty32

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

The Vagrant file mounts the local directory into the /ttl/sw directory, which is hardcoded and build into the sdb utilities.

The local directory also is mounted in /vagrant, which can be used to store the sdb files which need to be processed.


## Getting Started

Th key files are;

* bin/influx_migrate.sh -
* bin/influx_parse.py
* etc/migrate.cfg

## Work to do

* Create a single config file with sdb DATUM's and their name in
* Test pulling out all DATUM information from the sdb files (although this means creating 400+ manual liens of entry!)
* Test and automate the ingestion into INFLUX_DB


## Authors

* **Martina Zubac** - *Main developer*
* **Doug Arnold** - *Initial work*
* **Marco Lam** - *DB setup and support**
