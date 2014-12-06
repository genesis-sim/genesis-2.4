#!/usr/bin/env python

# plotPSD ver 0.0 - a command line utility to plot a wildcarded argument
# list of files containing time series data, and plot the
# Power Spectral Density (PSD) of them.
# This version averages the data from multiple files.

import sys, os
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab

def do_plot_files(filenames):
  if len(filenames) > 0:
      plotnum = 0
      for file in filenames:
          print file, plotnum
          print 'Reading %s' % file
          fp = open(file, 'r')
          count = len(fp.readlines())
          print count, " lines"
          fp.close()
          if (plotnum == 0):
              print plotnum
              tn = np.zeros(count)
              yn = np.zeros(count)
              print len(tn)
          fp = open(file, 'r')
          i=0
          for line in fp.readlines():
              data = line.split(" ")
              # Note that tn[i] is replaced, and yn[i] is addded to
              tn[i] = float(data[0]); yn[i] = yn[i] + float(data[1])
              i += 1
          plotnum += 1
  else:
    print "No files were specified for plotting!"
    print "Please give one or more filenames as arguments, e.g.\n"
    print "    plotSpectra EPSC_sum_0004sj.txt EPSC_sum_M0004*.txt \n"
    sys.exit()   

  # Now do the plotting of the averaged array data
  # Should check to be sure that the files are all in the same format

  dt = tn[1] - tn[0]
  # fourier sample rate
  fs = 1. / dt

  # Average the data over number of files
  yn /= plotnum

  npts = len(yn)
  startpt = int(0.2*fs)

  if (npts - startpt)%2!=0:
      startpt = startpt + 1

  yn = yn[startpt:]
  tn = tn[startpt:]
  nfft = len(tn)//4
  overlap = nfft//2

  print npts, nfft
  print startpt, len(yn)

  print 'Plottting average of ', plotnum, ' runs from series ', runid

  pxx,freqs=mlab.psd(yn,NFFT=nfft,Fs=fs,noverlap=overlap,window=mlab.window_none)
  pxx[0] = 0.0
  ax2.plot(freqs,pxx)
  print freqs[0], freqs[1], freqs[10], freqs[400]
  print pxx[0], pxx[1], pxx[10], pxx[400]

if __name__ == "__main__":
    # Get the arguments (possibly wildcarded) into a list of filenames
    filenames = sys.argv[1:]
    # Generate a RUNID from a string like "EPSC_sum_M0004E.txt"
    fn1 = sys.argv[1]
    fnbase,ext = os.path.splitext(fn1)
    # get string following final '_' and remove 1 char suffix
    runid = fnbase.split('_')[-1][:-1]

    print filenames, runid
    global tn
    global yn

    # create the plot
    fig = plt.figure()
    fig.canvas.set_window_title('G-3 Power Spectral Density Plot')
    fig.suptitle('Spectra Plot of average EPSCs')
    # A pleasant light blue background
    figbg = (191, 209, 212)
    fig.set_facecolor('#%02x%02x%02x' % figbg)
    #  fig.set_facecolor('ivory')

    ax2 = fig.add_subplot(111)

    do_plot_files(filenames)
    ax2.set_title('Spectral Density for ' + runid + ' series')
    # ax2.axis('auto')
    ax2.axis(xmin = 0, xmax = 50)
    ax2.set_ylabel('PSD (Amp^2/Hz)')  

    plt.draw()
    plt.show()
