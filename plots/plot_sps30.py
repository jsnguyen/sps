import os
from glob import glob
from datetime import datetime, timedelta

import numpy as np
import numpy.ma as ma
import matplotlib.pyplot as plt
import matplotlib.dates as mdates


class sps30_data():
    def __init__(self, filename):
        
        dtype = np.dtype([
            ("time", np.int_),
            ("mc_1p0", np.single),
            ("mc_2p5", np.single),
            ("mc_4p0", np.single),
            ("mc_10p0", np.single),
            ("nc_0p5", np.single),
            ("nc_1p0", np.single),
            ("nc_2p5", np.single),
            ("nc_4p0", np.single),
            ("nc_10p0", np.single),
            ("typical_particle_size", np.single),
        ])

        data = np.fromfile(filename, dtype=dtype)

        self.unix_times = data['time']
        self.utctimes = [datetime.utcfromtimestamp(el) for el in data['time']]
        self.times = [datetime.utcfromtimestamp(el)+timedelta(hours=-8) for el in data['time']]
        self.mc_1p0 = ma.masked_less(data['mc_1p0'], 0)
        self.mc_2p5 = ma.masked_less(data['mc_2p5'], 0)
        self.mc_4p0 = ma.masked_less(data['mc_4p0'], 0)
        self.mc_10p0 = ma.masked_less(data['mc_10p0'], 0)
        self.nc_0p5 = ma.masked_less(data['nc_0p5'], 0)
        self.nc_1p0 = ma.masked_less(data['nc_1p0'], 0)
        self.nc_2p5 = ma.masked_less(data['nc_2p5'], 0)
        self.nc_4p0 = ma.masked_less(data['nc_4p0'], 0)
        self.nc_10p0 = ma.masked_less(data['nc_10p0'], 0)
        self.typical_particle_size = ma.masked_less(data['typical_particle_size'], 0)


def rolling_average(a, n=10) :
    ret = np.cumsum(a, dtype=float)
    ret[n:] = ret[n:] - ret[:-n]
    return ret[n - 1:] / n

def plot_data(data, n_avg, output_filename=None):
    fig, axs = plt.subplots(3, 1, figsize=(20,10), sharex=True)
    fig.subplots_adjust(hspace=0.1)

    short_times = data.times[n_avg-1:]
    axs[0].plot(short_times, rolling_average(data.mc_1p0, n=n_avg), linestyle='-', label='PM$_{1.0}$')
    axs[0].plot(short_times, rolling_average(data.mc_2p5, n=n_avg), linestyle='-', label='PM$_{2.5}$')
    axs[0].plot(short_times, rolling_average(data.mc_4p0, n=n_avg), linestyle='-', label='PM$_{4.0}$')
    axs[0].plot(short_times, rolling_average(data.mc_10p0, n=n_avg), linestyle='-', label='PM$_{10.0}$')

    axs[1].plot(short_times, rolling_average(data.nc_0p5, n=n_avg), linestyle='-', label='$0.5 \mu m$')
    axs[1].plot(short_times, rolling_average(data.nc_1p0, n=n_avg), linestyle='-', label='$1.0 \mu m$')
    axs[1].plot(short_times, rolling_average(data.nc_2p5, n=n_avg), linestyle='-', label='$2.5 \mu m$')
    axs[1].plot(short_times, rolling_average(data.nc_4p0, n=n_avg), linestyle='-', label='$4.0 \mu m$')
    axs[1].plot(short_times, rolling_average(data.nc_10p0, n=n_avg), linestyle='-', label='$10.0 \mu m$')

    axs[2].plot(short_times, rolling_average(data.typical_particle_size, n=n_avg), linestyle='-')

    axs[0].set_title(data.times[-1].strftime('Last Reading: %Y-%m-%d %H:%M:%S'))

    #axs[2].set_xlabel('Time')

    axs[0].set_ylabel('Mass Concentration [$\mu g/m^3$]')
    axs[1].set_ylabel('Number Concentration [$\#/cm^3$]')
    axs[2].set_ylabel('Typical Particle Size [$\mu m$]')

    axs[0].tick_params(bottom=False)
    axs[1].tick_params(bottom=False)
    
    axs[0].legend()
    axs[1].legend()

    xlocator = mdates.MinuteLocator(byminute=[0,30], interval = 1)

    for tick in axs[2].get_xticklabels():
        tick.set_rotation(45)

    for ax in axs:
        ax.xaxis.set_major_locator(xlocator)
        ax.xaxis.set_major_formatter(mdates.DateFormatter('%I:%M %p'))
        ax.set_xlim(short_times[0], short_times[-1])
        ax.grid()

    if output_filename is not None:
        plt.savefig(output_filename, bbox_inches='tight')
    
    else:
        plt.show()

def get_latest(search_dir):
    return sorted(glob(search_dir+'*_sps30.bin'))[-1]

def main():

    search_dir = '/home/espresso/sps/pcount/data/'
    latest_filename = get_latest(search_dir)
    latest_output_filename = os.path.basename(latest_filename[:-4])+'.png'
    print('Plotting: {} -> {}'.format(latest_filename, latest_output_filename))

    data = sps30_data(latest_filename)

    n_avg = 200
    plot_data(data, n_avg, output_filename=latest_output_filename)

if __name__=='__main__':
	main()
