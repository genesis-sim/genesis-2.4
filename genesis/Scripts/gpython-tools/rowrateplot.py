#!/usr/bin/env python

# rowrateplot - a command line utility to generate spike frequency vs.
# time for groups of neurons.  It takes a single filename
# as argument, with a line for each time step containing the simulation
# time, followed by the average spike frequency during that time step for
# each group of cells.  Typically each cell group is a horizontal row of
# cells in the network.  Time is plotted on the x-axis, cell group (row)
# on the y-axis, with the frequency represented by a colored filled contour
# plot.

import sys, os
import matplotlib.pyplot as plt
import numpy as np

def plot_file():
    print 'Plotting %s' % filename
    fp = open(filename, 'r')
 
    # The first line is a header with the total number of lines in file
    # (nlines) and number of values/line (ncols).  The remaining lines
    # contain data, one line for each time interval.

    firstline = fp.readline()
    header = firstline.split()
    # Convert these strings to integers
    nlines = int(header[0])
    ncols = int(header[1])
    # nlines includes the header
    ntimes = nlines - 1
    # first column is time, the rest are data for groups 1 through ngroups
    ngroups = ncols - 1

    # print "setting up array"

    # set up firing frequency matrix of times and cell groups and intialize
    # with zeroes.  The firing frequencies for each of the ngroups cell
    # groups will be added for the next time as each line is read

    freq_array = np.zeros((ntimes,ngroups))

    # The x-axis will display the time values read from the file and added
    xvalues = []
    # The y-axis will display the group number, from 1 through ngroups
    yvalues = range(1,ngroups + 1,1)

    line_num = 0 # following lines will have data
    # print "array filled with zeroes"
    for line in fp.readlines():
        data = line.split()
        xvalues.append(float(data[0]))  # first item is the time
        # If time = 0, the data is likely to be meaningless,
        # so leave the row filled with zeros, instead of adding the data
        if (float(data[0]) <= 0.0):
            line_num += 1
            continue
        # This adds the row of data following the time into the array
        freq_array[line_num,0:ngroups] = data[1:ncols]
        line_num += 1

    # print "Matrix set up"

    # For contour plotting the matrix X-axis is the time, and Y-axis
    # are the target row (cell group) indices.
    # for np arrays, the first index refers to x-coordinates (time)
    # matplotlib contour plots reverse the indices and need a transpose

#    cplot = plt.contourf(xvalues, yvalues, freq_array.transpose())
    cplot = plt.contourf(xvalues, yvalues, freq_array.transpose(),15)
    # print "Plot created."
    lx = plt.xlabel("time (sec)")
    ly = plt.ylabel("Input Row")
    title = plt.title("Average spike frequency: "+filename)
    plt.axis(ymin=1, ymax=ngroups)
    # An alternate color map
    # cm = plt.hot()
    cb = plt.colorbar()
    print "Plot finished"

if __name__ == "__main__":
    '''
       rowrateplot - a command line utility to generate contour maps of
       spike frequency vs. simulation time for groups of neurons.  It takes
       a single filename as argument, with a line for each time step
       containing the simulation time, followed by the average spike
       frequency (firing rate) during that time step for each group of
       cells.  Typically each cell group is a horizontal row of cells in
       the network that are receiving auditory input.  Time is plotted on
       the x-axis, cell group (row) on the y-axis, with the average firing
       frequency of cells in that row at that time represented by a colored
       filled contour plot.

       The input file is typically produced with a GENESIS asc_file
       object, and should have a header line containing the integers

       nlines ncols

       with the total number of lines in the file, and the number of
       columns of whitespace-separated columns of data on each line.  The
       first data item on a line is the simulation time.  The time for each
       following line will differ by the width of the bin used to collect
       average firing frequencies.

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
            print 'Usage: freqplot filename'
            sys.exit()
    except:
        print 'Bad file, or other error'
        sys.exit()
    plt.show()

