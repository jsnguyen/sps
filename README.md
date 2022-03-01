# SPS

This is a reorganization of the [Sensirion SPS30 library](https://github.com/Sensirion/embedded-uart-sps) so that it can be used as a static library. I also wrote a logging script called `pcount`, which is meant to be used on startup with `systemd` services. See [https://sensirion.com/products/catalog/SPS30/](https://sensirion.com/products/catalog/SPS30/) for datasheets and info.

## pcount
`pcount` logs data every second to a file in the format `YYYY-MM-DD_sps30.bin`. The default data folder is `pcount/data`.

Each measurement is 24 bytes long in the following structure:

```
time    ,  pm1.0,  pm2.5,  pm4.0, pm10.0,  nc0.5,  nc1.0,  nc2.5,  nc4.0, nc10.0,  particle size
long int,  float,  float,  float,  float,  float,  float,  float,  float,  float,  float
```
The mass concentration (PMx.x) is in units of micrograms per meter cubed. The number concentration is in units of particles per meter cubed. The average particle size is in units of micrometers.

The first 30 measurements are tossed since they aren't reliable according to the data sheets. All subsequent measurements are okay.

A marker is inserted on startup by setting all of the measurements to -1 so that we can keep track of when the script starts. Otherwise, if the sensor reports bad data, it is not logged. If the script restarts, data is appended to the existing file.

See the comments in `pcount/pcount.c` for more detail about the script.

## Installation

To install:
```
make
sudo make install
```

## Uninstallation

To remove:
```
make clean
sudo make uninstall
```

## Usage

To run:
```
pcount
```
Typically this is run as a `systemd` executable, but the raw output of the script may be useful to test if it is working or not.
