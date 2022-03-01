# SPS

This is a reorganization of the [Sensirion SPS30 library](https://github.com/Sensirion/embedded-uart-sps) so that it can be used as a static library. I also wrote a logging script called `pcount`, which is meant to be used on startup with `systemd` services. See [https://sensirion.com/products/catalog/SPS30/](https://sensirion.com/products/catalog/SPS30/) for datasheets and info.

## pcount
`pcount` logs data every second to a file in the format `YYYY-MM-DD_sps30.bin`. The default data folder is `pcount/data`.

Each measurement is 24 bytes long in the following structure:

```
time    ,  pm1.0,  pm2.5,  pm4.0, pm10.0,  nc0.5,  nc1.0,  nc2.5,  nc4.0, nc10.0,  particle size
long int,  float,  float,  float,  float,  float,  float,  float,  float,  float,  float
```

The mass concentration (PMx.x) is in units of micrograms per meter cubed. The number concentration (NCx.x) is in units of particles per centimeter cubed. The average particle size is in units of micrometers.

The first 30 measurements after startup are tossed since they aren't reliable according to the data sheets. All subsequent measurements are okay.

A marker is inserted on startup by setting all of the measurements to -1 so that we can keep track of when the script starts. Otherwise, if the sensor reports bad data, it is not logged. If the script restarts, data is appended to the existing file.

See the comments in `pcount/pcount.c` for more detail about the script.

## Plots

Make plots with:

```
cd plots
python3 plot_sps30.py
```

Plotting requires `python3.6+`, `matplotlib` and `numpy`.

This plots a time series of all the data collected for the most recent day. Outputs a filename of the format `YYYY-MM-DD_sps30.png` in the folder that you run your script in. By default, it uses a rolling average of the last 200 points.

This is typically run as a cron job at the end of the day, right before 12am.

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
Typically this is run as a `systemd` service, but the raw output of the script may be useful to test if it is working or not.
