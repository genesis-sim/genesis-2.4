#!/usr/bin/env python

# plotPSD0 ver 0.0 - a command line utility to plot a wildcarded argument
# list of files containing time series data, and plot the
# Power Spectral Density (PSD) of them.
# This version overplots the data from multiple files.

import sys, os
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab

def plot_file(file,format):
    print 'Plotting %s' % file
    t = []; y = []
    fp = open(file, 'r')
    for line in fp.readlines():
        data = line.split(" ")
        t.append(data[0]); y.append(data[1])
    # Unlike normal pylab plots which can use lists as well as arrays,
    # the PSD routine needs a numpy array
    yn = np.array(y, dtype='d')
    tn = np.array(t, dtype='d')
    dt = tn[1] - tn[0]
    # fourier sample rate
    fs = 1. / dt

    npts = len(yn)
    startpt = int(0.2*fs)
    # nfft = 2*int((npts - startpt)/2.0)

    if (npts - startpt)%2!=0:
        startpt = startpt + 1

    yn = yn[startpt:]
    tn = tn[startpt:]

    nfft = len(tn)//4
    overlap = nfft//2

    print npts, nfft
    print startpt, len(yn)
    print np.shape(yn), np.size(yn)

    pxx,freqs=mlab.psd(yn,NFFT=nfft,Fs=fs,noverlap=overlap,window=mlab.window_none)
    pxx[0] = 0.0
    ax2.plot(freqs,pxx)
    print freqs[0], freqs[1], freqs[10], freqs[400]
    print pxx[0], pxx[1], pxx[10], pxx[400]

def do_plot_files(filenames):
    if len(filenames) > 0:
        formats = ['k', 'r', 'b', 'g', 'm', 'c']
        plotnum = 0
        for file in filenames:
            print file
            format = formats[plotnum % len(formats)]
            print format, plotnum
            try:
                if os.path.exists(file):
                    plot_file(file,format)
                    plotnum = plotnum + 1
                else:
                    print '*** Error: Incorrect file name or path specified ***'
            # I need to do better error handling!
            except:
                print 'An error ocurred'
                sys.exit()
    else:
        print "No files were specified for plotting!"
        print "Please give one or more filenames as arguments, e.g.\n"
        print "    plotSpectra EPSC_sum_0004sj.txt EPSC_sum_M0004*.txt \n"
        sys.exit()   
if __name__ == "__main__":
    # Get the arguments (possibly wildcarded) into a list of filenames
    filenames = sys.argv[1:]
    # Generate a RUNID from a string like "EPSC_sum_M0004E.txt"
    fn1 = sys.argv[1]
    fnbase,ext = os.path.splitext(fn1)
    # get string following final '_' and remove 1 char suffix
    runid = fnbase.split('_')[-1][:-1]

    print filenames, runid

    # create the plot
    fig = plt.figure()
    fig.canvas.set_window_title('G-3 Power Spectral Density Plot')
    fig.suptitle('G-3 Spectra Plot')
    # A pleasant light blue background
    figbg = (192, 208, 212)
    fig.set_facecolor('#%02x%02x%02x' % figbg)
    #  fig.set_facecolor('ivory')

    ax2 = fig.add_subplot(111)

    do_plot_files(filenames)

    ax2.set_title('Spectral Density for ' + runid + ' series')

    # ax2.axis('auto')
    ax2.axis(xmin = 0, xmax = 50)
    ax2.set_ylabel('PSD (Amp^2/Hz)')  
    # to add a legend
    # axes.legend(filenames)

    plt.draw()
    plt.show()
