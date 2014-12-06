#!/usr/bin/env python

# weight_hist.py ver 1.0 - a command line utility to plot a wildcarded argument
# list of files containing a line of connection weight values as a histogram.

import sys, os
import matplotlib.pyplot as plt
import numpy as np

def print_help():
    msg = """
    weight_hist.py ver 1.0 - a utility to plot synaptic weight histograms

    Usage: weight_hist.py filenames [filenames2] [filenames3] ...

    weight_hist.py plots a histogram of synaptic weight values for the
    connections to a synaptically activated channel, such as a GENESIS
    synchan element. Here 'filenames' is a wildcarded list of filenames, e.g.
    'weight_hist W128ex32inh-gmax0.2*.txt', or a single filename. Each
    file should consist of a single line of synaptic weight values.
    If there is more than one file, the data are plotted as separately
    colored and labeled bars.
    """
    print msg

def do_plot_files(filenames):
        formats = ['r', 'g', 'b', 'k', 'm', 'c']
        plotnum = 0
        datasets = []
        colors = []
        for file in filenames:
            # print file
            format = formats[plotnum % len(formats)]
            if os.path.exists(file):
                    fp = open(file, 'r')
                    values = np.loadtxt(fp,  dtype='float')
                    print format, plotnum
                    nvalues = len(values)
                    print 'Number of values = %d' % nvalues
                    datasets.append(values)
                    colors.append(format)
                    ndatasets = len(datasets)
                    print 'Number of datasets = %d' % ndatasets
                    print 'Plotting %s' % file
                    plotnum = plotnum + 1
            else:
                    print '*** Error: Incorrect file name or path specified ***'
                    sys.exit()
            # I need to do better error handling!
        # Now make the histogram with the data in values
        n, bins, patches = axes.hist(datasets, bins=20, label=filenames, color=colors)
        # print n, bins, patches

if __name__ == "__main__":
    # Get the arguments (possibly wildcarded) into a list of filenames
    filenames = sys.argv[1:]
    if len(filenames) == 0:
        print_help()
        sys.exit()
    print filenames
    # Generate a RUNID from a string like "W128ex32inh_0.20.txt"
    fn1 = sys.argv[1]
    fnbase,ext = os.path.splitext(fn1)
    if len(filenames) == 1:
        runid = fnbase
    else:
        # get string following final '_' and remove 1 char suffix
        # runid = 'series  ' +  fnbase.split('_')[-1][:-1]
        runid = 'series  ' +  fnbase[:-1]
#    print filenames, runid

    # create the plot
    fig = plt.figure()
    fig.canvas.set_window_title('GENESIS Weight Histogram')
    fig.suptitle('Histogram of connection weights')
    # A pleasant light blue background
    # figbg = (191, 209, 212)
    figbg = (160, 216, 248)
    fig.set_facecolor('#%02x%02x%02x' % figbg)
    axes = fig.add_subplot(111)
    do_plot_files(filenames)
#    n, bins, patches = axes.hist([values[0], values[1], values[2]],
#        bins=20, histtype='bar',color=format)
    axes.set_title('Weights for ' + runid)
    axes.set_xlabel('Synaptic Weights')
    axes.set_ylabel('Number')
    axes.legend(loc='upper center')
    plt.show()
