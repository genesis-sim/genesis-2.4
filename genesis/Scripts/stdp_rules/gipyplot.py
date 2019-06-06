#!/usr/bin/env python

"""
Creates a frame with a toolbar and graph for plotting
"""
import os
import pdb
import sys

import getopt
import optparse
import wx

from optparse import OptionParser

# Used to guarantee to use at least Wx2.8
# import wxversion
# wxversion.ensureMinimal('2.8')

import matplotlib
import numpy as np

# 
#matplotlib.use('WXAgg')

from matplotlib.backends.backend_wxagg import FigureCanvasWxAgg as FigureCanvas
from matplotlib.backends.backend_wxagg import NavigationToolbar2WxAgg

from matplotlib.backends.backend_wx import _load_bitmap
from matplotlib.figure import Figure
from numpy.random import rand


#----------------------------------------------------------------------

class MyNavigationToolbar(NavigationToolbar2WxAgg):
    """
    Extend the default wx toolbar with your own event handlers
    """
    ON_CUSTOM = wx.NewId()
    def __init__(self, canvas, cankill):
        NavigationToolbar2WxAgg.__init__(self, canvas)

        # for simplicity I'm going to reuse a bitmap from wx, you'll
        # probably want to add your own.
        # self.AddSimpleTool(self.ON_CUSTOM, _load_bitmap('matplotlib.png'),
        #                   'Click me', 'Activate custom contol')
        # wx.EVT_TOOL(self, self.ON_CUSTOM, self._on_custom)

#----------------------------------------------------------------------

    def _on_custom(self, evt):
        # add some text to the axes in a random location in axes (0,1)
        # coords) with a random color

        # get the axes
        ax = self.canvas.figure.axes[0]

        # generate a random location can color
        x,y = tuple(rand(2))
        rgb = tuple(rand(3))

        # add the text and draw
        ax.text(x, y, 'You clicked me',
                transform=ax.transAxes,
                color=rgb)
        self.canvas.draw()
        evt.Skip()

#*********************** Start G3Plot **********************************
class G3Plot(wx.Frame):

    """

    G3Plot creates a frame with a toolbar that holds a G-3 graph (2D y vs.
    x plot).  The graph allows multiple plots from multicolumn data 'x, y1,
    y2, ...'  given at successive x-values, given in one or more files.
    Wildcard file names are accepted.

    """
    def __init__(self, files, verbose=False,
                 title='(x,y) data', formt='k',
                 xlabel='X', ylabel='Y',
                 ymin=-0.1, ymax=0.05,
                 xmin=0.0, xmax=1.0,
                 pos=(50, 60), size=(640, 500)):
        
        wx.Frame.__init__(self, None, -1, "G3 Plot", pos, size)

        # self.SetBackgroundColour(wx.NamedColor("WHITE"))

        self.panel = None
        self.dpi = None
        self.fig = None
        self.canvas = None
        self.axes = None
        self.toolbar = None

        # Just in case we need to have these values stored
        self.verbose = verbose
        self.title = title
        self.formt = formt
        self.xlabel = xlabel
        self.ylabel = ylabel
        self.ymin = ymin
        self.ymax = ymax
        self.xmin = xmin
        self.xmax = xmax

        if self.verbose:
           print('xmin, xmax, ymin, ymax: ', self.xmin, self.xmax, self.ymin, self.ymax)

        self.subplot = []
        # Give the figure size in inches, and rez
        self.figure = Figure(figsize=(6.4,4.8), dpi=100)
        # Wait to create the subplot when AddSubplot() is called
#        self.axes = self.figure.add_subplot(1,1,1)
#        self.axes.set_title(self.title)
#        self.axes.set_xlabel(self.xlabel)
#        self.axes.set_ylabel(self.ylabel)

        self.PlotFiles(files) # create axes and plot all the data
        self.canvas = FigureCanvas(self, -1, self.figure)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.canvas, 1, wx.TOP | wx.LEFT | wx.EXPAND)
        # Capture the paint message
        # wx.EVT_PAINT(self, self.OnPaint)

        self.toolbar = MyNavigationToolbar(self.canvas, True)
        self.toolbar.Realize()
        if wx.Platform == '__WXMAC__':
            # Mac platform (OSX 10.3, MacPython) does not seem to cope with
            # having a toolbar in a sizer. This work-around gets the buttons
            # back, but at the expense of having the toolbar at the top
            self.SetToolBar(self.toolbar)
        else:
            # On Windows platform, default window size is incorrect, so set
            # toolbar width to figure width.
            tw, th = self.toolbar.GetSize()
            fw, fh = self.canvas.GetSize()
            # By adding toolbar in sizer, we are able to put it at the bottom
            # of the frame - so appearance is closer to GTK version.
            # As noted above, doesn't work for Mac.
            self.toolbar.SetSize(wx.Size(fw, th))
            self.sizer.Add(self.toolbar, 0, wx.LEFT | wx.EXPAND)

        # update the axes menu on the toolbar
        self.toolbar.update()
        self.SetSizer(self.sizer)
        self.Fit()

#----------------------------------------------------------------------

    def PlotFiles(self, file_list):
        """
        Loops through all files and makes a call to _PlotFile to plot
        each data set to the canvas.
        """
        self.plot_data = []

        if self.verbose:

            print("Processing %d files." % len(file_list))
            
        for f in file_list:

            this_file = os.path.abspath(f)

            if os.path.isfile(this_file):

                try:
                    
                    self._PlotFile(this_file)

                except Exception as e:

                    sys.exit("Error plotting files: %s" % e)
                
            else:

                print("File Error: '%s' is not found." % this_file)
                
#----------------------------------------------------------------------

    def _PlotFile(self, f):
        """
        Parses the data in file f and plots all data columns

        """
        
        # Will end up being a list of lists. First item
        # of the list will be the time step, all following
        # items will be output data at each time step.
        plot_data = None
        
        if self.verbose:

            print("Plotting file: %s" % f)
  

        this_file = os.path.abspath(f)

        if os.path.isfile(this_file):

            num_items = None
            
            fdata = open(this_file, 'r')

            for line in fdata.readlines():
                
                data = line.split()

                if plot_data is None:
                    
                    num_items = len(data)

                    plot_data = [[] for i in range(num_items)]

                for indx, d in enumerate(data):

                    if indx < num_items:
                        
                        try:
                        
                            plot_data[indx].append(d)

                        except IndexError as e:

                            print("Error processing data line for index %d" % indx)

            # Now we plot all of the data we collected.
            t = plot_data[0]

            for indx, x in enumerate(plot_data[1:]):

                if self.verbose:
                    num_plots = len(plot_data[1:])
                    print("\tPlotting data set %d of %d" % (indx+1, num_plots))

                xa = np.asarray(t,dtype=float); ya = np.asarray(x,dtype=float);
                self._AddSubplot(xa, ya)

        else:

            print("File Error: '%s' is not found." % this_file)

#----------------------------------------------------------------------

    def _AddSubplot(self, t, x):
        """
        Create a subplot if one doesn't exist, and plot the (t, x) data.
        For historical reasons the x-axis variable is called 't' and the
        y-axis variable 'x'.  This should be changed!

        The add_subplot method of a Figure creates an Axes instance
        (111) == (1,1,1) --> row 1, col 1, Figure 1
       
        """
        
        if self.axes is None:
            self.axes = self.figure.add_subplot(1,1,1)
            # This is where Axes attributes are set
            self.axes.set_title(self.title)
            self.axes.set_xlabel(self.xlabel)
            self.axes.set_ylabel(self.ylabel)

        if self.xmin is None or self.xmax is None:
            self.axes.set_autoscalex_on(True)
        else:
            self.axes.set_autoscalex_on(False)
            self.axes.axis(xmin=self.xmin,xmax=self.xmax)

        if self.ymin is None or self.ymax is None:
            self.axes.set_autoscaley_on(True)
        else:
            self.axes.set_autoscaley_on(False)
            self.axes.axis(ymin=self.ymin,ymax=self.ymax)

        # With the new, or an exising Axes instance, plot the data          
        if self.formt is None:
            self.axes.plot(t, x)
        else:
            self.axes.plot(t, x, self.formt)

#----------------------------------------------------------------------


    def OnPaint(self, event):
        self.canvas.draw()
        event.Skip()

#-------------------- End of class G3Plot -----------------------------------

#----------- plotApp is the main wx.App to parse command line and plot -------

class plotApp(wx.App):
    def OnInit(self):
        version = 'gipyplot Ver. 2.0'
        filenames = []
        print_version = False
        verbose = False
        formt = None
        xlabel = 'X'
        ylabel = 'Y'
        title="GENESIS 3 Plot"

        cwd=os.getcwd()
        #os.chdir(cwd)

        description = """

gipyplot creates a frame with a toolbar that holds a G-3 graph (a 2D y vs x
plot).  The graph allows multiple plots from multicolumn data 'x, y1, y2,
...'  given at successive x-values, given in one or more files.  Wildcard
file names are accepted.

        """
        usage = "%s [OPTIONS] <files>"
        parser = OptionParser(usage=usage, version=version, description=description)
        parser.add_option("-v", "--verbose", action="store_true", dest="verbose",
            default = False, help="show verbose output")
    
        parser.add_option("-V", "--show-version", action="store_true", dest="version_flag",
            default = False, help="print the program version")

        parser.add_option("-t", "--title", dest="title", type="string",
            default=title,  help="Title for the graph")

        parser.add_option("-f", "--format", dest="formt", type="string",
                      help="The plot format and color. e.g 'k' \
                      default: None cycles color for multiple plots")

        parser.add_option("-x", "--xlabel", dest="xlabel", type="string",
            default=xlabel, help="The label for the X axis")

        parser.add_option("-y", "--ylabel", dest="ylabel", type="string",
            default=ylabel, help="The label for the Y axis")

        rangegroup = optparse.OptionGroup(parser, "Axis Range Options: \n "
            ' Autoscaling is used unless both min and max values are given')

        rangegroup.add_option("--xmin", dest="xmin", type="float",
            default=None, help="Minimum value for the X axis")
        rangegroup.add_option("--xmax", dest="xmax", type="float",
            default=None, help="Maximum value for the X axis")
        rangegroup.add_option("--ymin", dest="ymin", type="float",
            default=None, help="Minimum value for the Y axis")
        rangegroup.add_option("--ymax", dest="ymax", type="float",
            default=None, help="Maximum value for the Y axis")
        parser.add_option_group(rangegroup)
        (options, args) = parser.parse_args()

        if options.version_flag is True:
            parser.print_version()
            return True

#        if len(args) < 1:
        
#             parser.error("Need at least one data file to read in")

        # collect options
        verbose = options.verbose
        if len(args) > 0:
            for a in args:
                if os.path.isfile( a ):
                    filenames.append(a)
                else:
                    parser.error("File '%s' doesn't exist" % a)

            formt = options.formt
            xlabel = options.xlabel
            ylabel = options.ylabel
            title = options.title

        # Now make the plot
        frame = G3Plot(files=filenames,
            verbose=verbose,
            xlabel=xlabel,
            ylabel=ylabel,
            title=title,
            formt=formt,
            xmin=options.xmin,
            xmax=options.xmax,
            ymin=options.ymin,
            ymax=options.ymax)

        frame.Show()
        self.SetTopWindow(frame)
        return True

#----------------------------------------------------------------------
            
if __name__ == '__main__':
    app = plotApp(0)
    app.MainLoop()
