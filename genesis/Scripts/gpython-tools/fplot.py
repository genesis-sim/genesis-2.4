#!/usr/bin/env python

import sys, os
import pylab

def plot(filename):
    print 'Plotting %s' % filename
    pylab.plotfile(filename, cols=(0,1), delimiter=' ', subplots=False)
    pylab.show()

if __name__ == "__main__":
    try:
        filename = sys.argv[1]
        if os.path.exists(filename):            
            plot(filename)
        else:
            print '**Error: Incorrect file name or path specified.'
    except:
        print 'incorrect filename'
        sys.exit()

    
