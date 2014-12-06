#!/usr/bin/env python

# rasterplot - a command line utility to generate rasterplots of
# firing times for a group of neurons.  It takes a single filename
# as argument, with a line for each cell containing the spike times separated
# by spaces.  The times are plotted as dots with the time on the x-axis and
# the cell number (the line number) on the y-axis.


import sys, os
import matplotlib.pyplot as plt

def plot_file():
    print 'Plotting %s' % filename
    fp = open(filename, 'r')
    format = 'b.'
    cell_num = 0
    for line in fp.readlines():
        cell_num += 1
        y = cell_num
        data = line.split()
        # print data[0], data[1], data[2], data[3]
        for x in data:
            axes.plot(x, y, format)
    print "Processing finished"

if __name__ == "__main__":
    '''
    rasterplot - a command line utility to generate rasterplots of firing
    times for a group of neurons.  It takes a single filename as argument,
    with a line for each cell containing the spike times separated by
    spaces.  The times are plotted as dots with the time on the x-axis and
    the cell number (the line number) on the y-axis.
    '''
    # create the plot and set size of the plot figure
    fig = plt.figure(dpi=120)
    axes = fig.add_subplot(111)
    try:
        filename = sys.argv[1]
        if os.path.exists(filename):            
            plot_file()
        else:
            print '**Error: Incorrect file name or path specified.\n'
            print 'Usage: rasterplot filename'
            sys.exit()
    except:
        print 'Bad file, or other error'
        sys.exit()
    axes.set_title('Raster Plot: '+filename)
    axes.set_xlabel('Seconds')
    axes.set_ylabel('Cell Number')
    plt.draw()
    plt.show()

