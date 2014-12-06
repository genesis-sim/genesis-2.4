#!/usr/bin/env python

import sys, os
import pylab

def process_file():
    print 'Reading %s' % filename
    fp = open(filename, 'r')
    for line in fp.readlines():
        data = line.split()
        # print data
        for x in data:
            print x,
        print
    print "Processing finished"

if __name__ == "__main__":
    try:
        filename = sys.argv[1]
        if os.path.exists(filename):            
            process_file()
        else:
            print '**Error: Incorrect file name!'
    except:
        print '**Error: Exception raised!'
        sys.exit()
