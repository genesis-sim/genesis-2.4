#!/usr/bin/env python

# plotVm ver 0.5 - a command line utility to plot a wildcarded argument
# list of files containing membrane potential data, and plots them in
# different colors on the same axes

import sys, os
import matplotlib.pyplot as plt
import numpy as np

def plot_file(file,format):
    print 'Plotting %s' % file
    x = []; y = []
    fp = open(file, 'r')
    for line in fp.readlines():
        data = line.split(" ")
        x.append(data[0]); y.append(data[1])
    # print "Data length is ", len(x), "Format is ", format
    axes.plot(x, y, format)
    # use this instead, to let pyplot plot pick new colors
    # axes.plot(x, y)

def do_plot_files(filenames):
    if len(filenames) > 0:
        formats = ['k', 'r', 'b', 'g', 'm', 'c']
        plotnum = 0
        for file in filenames:
            # print file
            format = formats[plotnum % len(formats)]
            # print format, plotnum
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
        print "    plotVm Vm.out pyr4*.out\n"
        sys.exit()   
if __name__ == "__main__":
    # Get the arguments (possibly wildcarded) into a list of filenames
    filenames = sys.argv[1:]
    print filenames
    # create the plot
    fig = plt.figure()
    axes = fig.add_subplot(111)
    do_plot_files(filenames)
    axes.set_title('Membrane Potential')
    axes.set_xlabel('seconds')
    axes.set_ylabel('Volts')
    axes.axis(ymin=-0.1, ymax=0.05)
    # to use autoscaling
    # axes.axis('auto')
    # to add a legend
    # axes.legend(filenames)
    plt.draw()
    plt.show()
