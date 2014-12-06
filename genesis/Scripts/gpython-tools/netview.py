#!/usr/bin/env python

# Basic wxPython frame to hold a matplotlib figure for plotting
# It defines some basic menu items with bindings to functions to execute

import sys, os, glob, time, math, bz2

# import needed wxPython modules

import wx
import wx.html
import	wx.lib.dialogs
from wx.lib.stattext import GenStaticText

import matplotlib
import matplotlib.cm as cm
from matplotlib.widgets import Slider, Button

matplotlib.use('WXAgg')

from matplotlib.figure import Figure
from matplotlib.backends.backend_wxagg import \
    FigureCanvasWxAgg as FigCanvas, \
    NavigationToolbar2WxAgg as NavigationToolbar
import matplotlib.font_manager as font_manager

import numpy as np

# Class for fancy labeled text entry widget
class XDialog(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
	wx.Panel.__init__(self, parent, *args, **kwargs)
	self.entry_label = GenStaticText(self, wx.ID_ANY,
            label=" default label ", size=(-1,30))
	# Use the default width (-1) and 30 pixel height
	# Set the background color to palegoldenrod
	self.entry_label.SetBackgroundColour('#EEE8AA')

	self.entry = wx.TextCtrl(self, wx.ID_ANY, '')
        self.entry.SetBackgroundColour('#EEE8AA')
	self.entry.SetWindowStyleFlag(wx.TE_PROCESS_ENTER)

        # Arrange the widgets in a horizontal BoxSizer
	dlg_sizer = wx.BoxSizer(wx.HORIZONTAL)

	dlg_sizer.Add(self.entry_label, 1, border=0)
	dlg_sizer.Add(self.entry, 1, wx.EXPAND, border=0)

	self.SetSizer(dlg_sizer)
    
# custom frame to hold the panel with a Figure and WxAgg backend canvas
class PlotFrame(wx.Frame):
    """
        PlotFrame is a custom wxPython frame to hold the panel with a
        Figure and WxAgg backend canvas for matplotlib plots or other
        figures.  In this frame:

        self is an instance of a wxFrame;
        axes is an instance of MPL Axes;
        fig is an instance of MPL Figure;
        panel is an instance of wxPanel, used for the main panel, to hold
        canvas, an instance of MPL FigureCanvasWxAgg.
    """

    # Main function to set everything up when the frame is created
    def __init__(self, title, pos, size):

        """
           This will be executed when an instance of PlotFrame is created.
           It is the place to define any globals as "self.<name>".
        """
        wx.Frame.__init__(self, None, wx.ID_ANY, title, pos, size)

        if len(sys.argv) < 2:
            self.filename = ""
        else:
            self.filename =  sys.argv[1]

        # set some Boolean flags
        self.STOP = False
        self.data_loaded = False
        self.reverse_play = False

        self.step = 1

        #    Make the main Matplotlib panel for plots
        self.create_main_panel()  # creates canvas and contents

        # Then add wxPython widgets below the MPL canvas
	# Layout with box sizers

        self.sizer = wx.BoxSizer(wx.VERTICAL)
	self.sizer.Add(self.canvas, 1, wx.LEFT | wx.TOP | wx.EXPAND)
	self.sizer.AddSpacer(10)
	self.sizer.Add(self.toolbar, 0, wx.EXPAND)
	self.sizer.AddSpacer(10)

        #    Make the control panel with a row of buttons
        self.create_button_bar()
        self.sizer.Add(self.button_bar_sizer, 0, flag = wx.ALIGN_CENTER | wx.TOP)

        #    Make a Status Bar
        self.statusbar = self.CreateStatusBar()
        self.sizer.Add(self.statusbar, 0, wx.EXPAND)

        self.SetStatusText("Frame created ...")

        # -------------------------------------------------------
        #	       set up the Menu Bar
        # -------------------------------------------------------
        menuBar = wx.MenuBar()

        menuFile = wx.Menu() # File menu
        menuFile.Append(1, "&Open", "Filename(s) or wildcard list to plot")
        menuFile.Append(3, "Save", "Save plot as a PNG image")
        menuFile.AppendSeparator()
        menuFile.Append(10, "E&xit")
        menuBar.Append(menuFile, "&File")

        menuHelp = wx.Menu() # Help menu
        menuHelp.Append(11, "&About Netview")
        menuHelp.Append(12, "&Usage and Help")
        menuHelp.Append(13, "Program &Info")

        menuBar.Append(menuHelp, "&Help")
        self.SetMenuBar(menuBar)

        self.panel.SetSizer(self.sizer)
        self.sizer.Fit(self)

        # -------------------------------------------------------
        #      Bind the menu items to functions
        # -------------------------------------------------------

        self.Bind(wx.EVT_MENU, self.OnOpen, id=1)
        self.Bind(wx.EVT_MENU, self.OnSave, id=3)
        self.Bind(wx.EVT_MENU, self.OnQuit, id=10)
        self.Bind(wx.EVT_MENU, self.OnAbout, id=11)
	self.Bind(wx.EVT_MENU, self.OnUsage, id=12)
        self.Bind(wx.EVT_MENU, self.OnInfo, id=13)
	
        # methods defined below to get and plot the data
        # Normally do the plot on request, and not here
        # self.get_data_params()
        # self.init_plot()
        # self.get_xyt_data()
        # plot_data()

	# ---------- end of __init__ ----------------------------

    # -------------------------------------------------------
    #   Function to make the main Matplotlib panel for plots
    # -------------------------------------------------------

    def create_main_panel(self):
        """ create_main_panel creates the main mpl panel with instances of:
             * mpl Canvas 
             * mpl Figure 
             * mpl Figure
             * mpl Axes with subplot
             * mpl Widget class Sliders and Button
             * mpl navigation toolbar
           self.axes is the instance of MPL Axes, and is where it all happens
        """

        self.panel = wx.Panel(self)
        
        # Create the mpl Figure and FigCanvas objects. 
        # 3.5 x 5 inches, 100 dots-per-inch
        #
        self.dpi = 100
        self.fig = Figure((3.5, 5.0), dpi=self.dpi)
        self.canvas = FigCanvas(self.panel, wx.ID_ANY, self.fig)
        
        # Since we have only one plot, we could use add_axes 
        # instead of add_subplot, but then the subplot
        # configuration tool in the navigation toolbar wouldn't work.


        self.axes = self.fig.add_subplot(111)
        # (111) == (1,1,1) --> row 1, col 1, Figure 1)
        # self.axes.set_title("View from: "+self.filename)

        # Now create some sliders below the plot after making room
        self.fig.subplots_adjust(left=0.1, bottom=0.20)

        self.axtmin = self.fig.add_axes([0.2, 0.10, 0.5, 0.03])
        self.axtmax = self.fig.add_axes([0.2, 0.05, 0.5, 0.03])

        self.stmin = Slider(self.axtmin, 't_min:', 0.0, 1.0, valinit = 0.0)
        self.stmax = Slider(self.axtmax, 't_max:', 0.0, 1.0, valinit = 1.0)
        self.stmin.on_changed(self.update_trange)
        self.stmax.on_changed(self.update_trange)

        self.axbutton = self.fig.add_axes([0.8, 0.07, 0.1, 0.07])
        self.reset_button = Button(self.axbutton, 'Reset')
        self.reset_button.color = 'skyblue'
        self.reset_button.hovercolor = 'lightblue'
        self.reset_button.on_clicked(self.reset_trange)

        # Create the navigation toolbar, tied to the canvas

        self.toolbar = NavigationToolbar(self.canvas)

    def update_trange(self, event):
        self.t_min = self.stmin.val
        self.t_max = self.stmax.val
        # print(self.t_min, self.t_max)

    def reset_trange(self, event):
        self.stmin.reset()
        self.stmax.reset()

    def create_button_bar(self):
	"""
	create_button_bar makes a control panel bar with buttons and
	toggles for

	New Data - Play - STOP - Single Step - Forward/Back - Normal/Fast

	It does not create a Panel container, but simply creates Button
	objects with bindings, and adds  them to a horizontal BoxSizer
	self.button_bar_sizer.	This is added to the PlotFrame vertical
	BoxSizer, after the MPL canvas, during initialization of the frame.

	"""
	rewind_button = wx.Button(self.panel, -1, "New Data")
	self.Bind(wx.EVT_BUTTON, self.OnRewind, rewind_button)

	replot_button = wx.Button(self.panel, -1, "Play")
	self.Bind(wx.EVT_BUTTON, self.OnReplot, replot_button)

	sstep_button = wx.Button(self.panel, -1, "Single Step")
	self.Bind(wx.EVT_BUTTON, self.OnSstep, sstep_button)

	stop_button = wx.Button(self.panel, -1, "STOP")
	self.Bind(wx.EVT_BUTTON, self.OnStop, stop_button)

	# The toggle buttons need to be globally accessible

	self.forward_toggle = wx.ToggleButton(self.panel, -1, "Forward")
	self.forward_toggle.SetValue(True)
	self.forward_toggle.SetLabel("Forward")
	self.Bind(wx.EVT_TOGGLEBUTTON, self.OnForward, self.forward_toggle)

	self.fast_toggle = wx.ToggleButton(self.panel, -1, " Normal ")
        self.fast_toggle.SetValue(True)
        self.fast_toggle.SetLabel(" Normal ")
        self.Bind(wx.EVT_TOGGLEBUTTON, self.OnFast, self.fast_toggle)

        # Set button colors to some simple colors that are likely
        # to be independent on X11 color definitions.  Some nice
        # bit maps (from a media player skin?) should be used
        # or the buttons and toggle state colors in OnFast() below

        rewind_button.SetBackgroundColour('skyblue')
        replot_button.SetBackgroundColour('skyblue')
        sstep_button.SetBackgroundColour('skyblue')
        stop_button.SetBackgroundColour('skyblue')
        self.forward_toggle.SetForegroundColour('black')
        self.forward_toggle.SetBackgroundColour('yellow')
        self.fast_toggle.SetForegroundColour('black')
        self.fast_toggle.SetBackgroundColour('yellow')
        self.button_bar_sizer = wx.BoxSizer(wx.HORIZONTAL)
        flags = wx.ALIGN_CENTER | wx.ALL
        self.button_bar_sizer.Add(rewind_button, 0, border=3, flag=flags)
        self.button_bar_sizer.Add(replot_button, 0, border=3, flag=flags)
        self.button_bar_sizer.Add(sstep_button, 0, border=3, flag=flags)
        self.button_bar_sizer.Add(stop_button, 0, border=3, flag=flags)
        self.button_bar_sizer.Add(self.forward_toggle, 0, border=3, flag=flags)
        self.button_bar_sizer.Add(self.fast_toggle, 0, border=3, flag=flags)

    # -------------------------------------------------------
    #	Functions to generate or read (x,y) data and plot it
    # -------------------------------------------------------

    def get_data_params(self):
        #  These parameters  would normally be provided in a file header,
        # past as arguments in a function, or from other file information
        #  Next version will bring up a dialog for dt NX NY if no file header

        # Here check to see if a filename should be entered from File/Open
        # self.filename = 'Ex_net_Vm_0001.txt'
        if len(self.filename) == 0:
            # fake a button press of File/Open
            self.OnOpen(wx.EVT_BUTTON)

        # should check here if file exists as specified [path]/filename

        # assume it is a bzip2 compressed file
        try:
      	    fp = bz2.BZ2File(self.filename)
	    line = fp.readline()
        except IOError:
            # then assume plain text
            fp = open(self.filename)
	    line = fp.readline()
       
        fp.close()

        # check if first line is a header line starting with '#'
        header = line.split()
        if header[0][0] == "#": 
            self.Ntimes = int(header[1])
            self.t_min =  float(header[2])
            self.dt = float(header[3])
            self.NX = int(header[4])
            self.NY = int(header[5])
        else:
            pdentry = self.ParamEntryDialog()
            if pdentry.ShowModal() ==  wx.ID_OK:
                self.Ntimes = int(pdentry.Ntimes_dialog.entry.GetValue())
		self.t_min = float(pdentry.tmin_dialog.entry.GetValue())
		self.dt = float(pdentry.dt_dialog.entry.GetValue())
                self.NX	= int(pdentry.NX_dialog.entry.GetValue())
                self.NY = int(pdentry.NY_dialog.entry.GetValue())
                print 'Ntimes = ', self.Ntimes, ' t_min = ', self.t_min
                print 'NX = ', self.NX, ' NY = ', self.NY
            pdentry.Destroy()
        self.t_max = (self.Ntimes - 1)*self.dt
        # reset slider max and min
        self.stmin.valmax = self.t_max
        self.stmin.valinit = self.t_min
        self.stmax.valmax = self.t_max
        self.stmax.valinit = self.t_max
        self.stmax.set_val(self.t_max)
        self.stmin.reset()
        self.stmax.reset()
        fp.close()

    def init_plot(self):
        ''' 
        init_plot creates the initial plot display. A normal MPL plot
	would be created here with a command "self.axes.plot(x, y)" in
	order to create a plot of points in the x and y arrays on the
	Axes subplot.  Here, we create an AxesImage instance with
	imshow(), instead.  The initial image is a blank one of the
	proper dimensions, filled with zeroes.

        '''
        self.t_max = (self.Ntimes - 1)*self.dt
        self.axes.set_title("View of "+self.filename)
        # Note that NumPy array (row, col) = image (y, x)
        data0 = np.zeros((self.NY,self.NX))

        # Define a 'cold' to 'hot' color scale based in GENESIS 2 'hot'
        hotcolors = ['#000032', '#00003c', '#000046',
        '#000050', '#00005a', '#000064', '#00006e', '#000078', '#000082',
        '#00008c', '#000096', '#0000a0', '#0000aa', '#0000b4', '#0000be',
        '#0000c8', '#0000d2', '#0000dc', '#0000e6', '#0000f0', '#0000fa',
        '#0000ff', '#000af6', '#0014ec', '#001ee2', '#0028d8', '#0032ce',
        '#003cc4', '#0046ba', '#0050b0', '#005aa6', '#00649c', '#006e92',
        '#007888', '#00827e', '#008c74', '#00966a', '#00a060', '#00aa56',
        '#00b44c', '#00be42', '#00c838', '#00d22e', '#00dc24', '#00e61a',
        '#00f010', '#00fa06', '#00ff00', '#0af600', '#14ec00', '#1ee200',
        '#28d800', '#32ce00', '#3cc400', '#46ba00', '#50b000', '#5aa600',
        '#649c00', '#6e9200', '#788800', '#827e00', '#8c7400', '#966a00',
        '#a06000', '#aa5600', '#b44c00', '#be4200', '#c83800', '#d22e00',
        '#dc2400', '#e61a00', '#f01000', '#fa0600', '#ff0000', '#ff0a00',
        '#ff1400', '#ff1e00', '#ff2800', '#ff3200', '#ff3c00', '#ff4600',
        '#ff5000', '#ff5a00', '#ff6400', '#ff6e00', '#ff7800', '#ff8200',
        '#ff8c00', '#ff9600', '#ffa000', '#ffaa00', '#ffb400', '#ffbe00',
        '#ffc800', '#ffd200', '#ffdc00', '#ffe600', '#fff000', '#fffa00',
        '#ffff00', '#ffff0a', '#ffff14', '#ffff1e', '#ffff28', '#ffff32',
        '#ffff3c', '#ffff46', '#ffff50', '#ffff5a', '#ffff64', '#ffff6e',
        '#ffff78', '#ffff82', '#ffff8c', '#ffff96', '#ffffa0', '#ffffaa',
        '#ffffb4', '#ffffbe', '#ffffc8', '#ffffd2', '#ffffdc', '#ffffe6',
        '#fffff0']

        cmap = matplotlib.colors.ListedColormap(hotcolors)

        self.im = self.axes.imshow(data0, cmap=cmap, origin='lower')

        # http://matplotlib.sourceforge.net/examples/pylab_examples/show_colormaps.html
        # shows examples to use as a 'cold' to 'hot' mapping of value to color
        # cm.jet, cm.gnuplot and cm.afmhot are good choices, but are unlike G2 'hot'

        self.im.cmap=cmap

        # Not sure how to properly add a colorbar
        # self.cb = self.fig.colorbar(self.im, orientation='vertical')

        # refresh the canvas
        self.canvas.draw()      

    def get_xyt_data(self):
        # Create scaled (0-1) luminance(x,y) array from ascii G-2 disk_out file
        # get the data to plot from the specified filename
        # Note that NumPy loadtxt transparently deals with bz2 compression
        self.SetStatusText('Data loading - please wait ....')
        rawdata = np.loadtxt(self.filename)
        # Note the difference between NumPy [row, col] order and network
        # x-y grid (x, y) = (col, row). We want a NumPy NY x NX, not 
        # NX x NY, array to be used by the AxesImage object.

        xydata = np.resize(rawdata, (self.Ntimes, self.NY, self.NX))
        # imshow expects the data to be scaled to range 0-1.
        Vmin = xydata.min()
        Vmax = xydata.max()
        self.ldata = (xydata - Vmin)/(Vmax - Vmin)
        self.data_loaded = True
        self.SetStatusText('Data has been loaded - click Play')

    def plot_data(self):
        ''' plot_data() shows successive frames of the data that was loaded
            into the ldata array.  Creating a new self.im AxesImage instance
            for each frame is extremely slow, so the set_data method of
            AxesImage is used to load new data into the existing self.im for
            each frame.  Normally 'self.canvas.draw()' would be used to
            display a frame, but redrawing the entire canvas, redraws the
            axes, labels, sliders, buttons, or anything else on the canvas.
            This uses a method taken from an example in Ch 7, p. 192
            Matplotlib for Python developers, with draw_artist() and blit()
            redraw only the part that was changed.

        '''
        if self.data_loaded == False:
            # bring up a warning dialog
            msg = """
            Data for plotting has not been loaded!
            Please enter the file to plot with File/Open, unless
            it was already specified, and then click on 'New Data'
            to load the data to play back, before clicking 'Play'.
            """
            wx.MessageBox(msg, "Plot Warning", wx.OK | wx.ICON_ERROR,self)
            return

        # set color limits
        self.im.set_clim(0.0, 1.0)
        self.im.set_interpolation('nearest')
        # 'None' is is slightly faster, but not implemented for MPL ver < 1.1
        # self.im.set_interpolation('None')

        # do an initial draw, then save the empty figure axes
        self.canvas.draw()      

        # self.bg = self.canvas.copy_from_bbox(self.axes.bbox)
        # However the save and restore is only  needed if I change
        # axes legends, etc.  The draw_artist(artist), and blit
        # are much faster than canvas.draw() and are sufficient.

        print 'system time (seconds) = ', time.time()

        # round frame_min down and frame_max up for the time window
        frame_min = int(self.t_min/self.dt)
        frame_max = min(int(self.t_max/self.dt) + 1, self.Ntimes)
        frame_step = self.step

        # Displaying simulation time to the status bar is much faster
        # than updating a slider progress bar, but location isn't optimum.
        # The check for the STOP button doesn't work because the button
        # click is not registered until this function exits.

        # check to see if self.reverse_play == True
        # then interchange frame_min, frame_max, and use negative step
        if self.reverse_play == True:
            frame_min = min(int(self.t_max/self.dt) + 1, self.Ntimes) - 1
            frame_max = int(self.t_min/self.dt) - 1
            frame_step = -self.step
        for frame_num in range(frame_min, frame_max, frame_step):
            self.SetStatusText('time: '+str(frame_num*self.dt))
            if self.STOP == True:
                self.t_min = frame_num*self.dt
                # set t_min slider ?
                self.STOP = False
                break
            self.im.set_data(self.ldata[frame_num])
            self.axes.draw_artist(self.im)
            self.canvas.blit(self.axes.bbox)

        print 'system time (seconds) = ', time.time()

    #  ------------------------------------------------------------------
    #	Define the classes and functions for getting parameter values
    #  --------------------------------------------------------------

    class ParamEntryDialog(wx.Dialog):
        def __init__(self):
	    wx.Dialog.__init__(self, None, wx.ID_ANY)
	    self.SetSize((250, 200))
	    self.SetTitle('Enter Data File Parameters')
	    vbox = wx.BoxSizer(wx.VERTICAL)
	    self.Ntimes_dialog = XDialog(self)
	    self.Ntimes_dialog.entry_label.SetLabel('Number of entries')
	    self.Ntimes_dialog.entry.ChangeValue(str(2501))
	    self.tmin_dialog = XDialog(self)
	    self.tmin_dialog.entry_label.SetLabel('Start time (sec)')
	    self.tmin_dialog.entry.ChangeValue(str(0.0))

	    self.dt_dialog = XDialog(self)
	    self.dt_dialog.entry_label.SetLabel('Output time step (sec)')
	    self.dt_dialog.entry.ChangeValue(str(0.0002))

	    self.NX_dialog = XDialog(self)
	    self.NX_dialog.entry_label.SetLabel('Number of cells on x-axis')
            self.NX_dialog.entry.ChangeValue(str(32))
            self.NY_dialog = XDialog(self)
            self.NY_dialog.entry_label.SetLabel('Number of cells on y-axis')
            self.NY_dialog.entry.ChangeValue(str(32))

            vbox.Add(self.Ntimes_dialog, 0, wx.EXPAND|wx.ALL, border=5)
            vbox.Add(self.tmin_dialog, 0, wx.EXPAND|wx.ALL, border=5)
            vbox.Add(self.dt_dialog, 0, wx.EXPAND|wx.ALL, border=5)
            vbox.Add(self.NX_dialog, 0, wx.EXPAND|wx.ALL, border=5)
            vbox.Add(self.NY_dialog, 0, wx.EXPAND|wx.ALL, border=5)

            okButton = wx.Button(self, wx.ID_OK,'Ok')
	    # vbox.Add(okButton,flag=wx.ALIGN_CENTER|wx.TOP|wx.BOTTOM, border=10)
	    vbox.Add(okButton,flag=wx.ALIGN_CENTER, border=10)

            self.SetSizer(vbox)
	    self.SetSizerAndFit(vbox)

    #  ------------------------------------------------------------------
    #	Define the functions executed on menu choices
    #  ---------------------------------------------------------------
         
    def OnQuit(self, event):
        self.Close()

    def OnSave(self, event):
	file_choices = "PNG (*.png)|*.png"
        dlg = wx.FileDialog(
	    self, 
	    message="Save plot as...",
            defaultDir=os.getcwd(),
            defaultFile="plot.png",
            wildcard=file_choices,
            style=wx.SAVE)

        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            self.canvas.print_figure(path, dpi=self.dpi)
            # self.flash_status_message("Saved to %s" % path)
         
    def OnAbout(self, event):
	msg = """

                      G-3 Netview ver. 1.7

Netview is a stand-alone Python application for viewing
the output of GENESIS 2 and 3 network simulations.
It is intended to replace GENESIS 2 SLI scripts that use the
XODUS 'xview' widget.

The design and operation is based on the G3Plot application
for creating 2D plots of y(t) or y(x) from data files.
Unlike G3Plot, the image created with Netview is an animated
representation of a rectangular network with colored squares
used to indicate the value of some variable at that position
and time.  Typically, this would be the membrane potenial of
a cell soma, or a synaptic current in a dendrite segment.

Help/Usage gives HTML help for using Netview.
This is the main Help page.

Help/Program Info provides some information about the
objects and functions, and the wxPython and matplotlib
classes used here.

Dave Beeman, August 2012
	"""
        dlg = wx.MessageDialog(self, msg, "About G-3 Netview",
            wx.OK | wx.ICON_QUESTION)
	dlg.ShowModal()
	dlg.Destroy()

    def OnOpen(self, event):
        dlg = wx.TextEntryDialog(self, "File with x,y data to plot",
	    "File Open", self.filename, style=wx.OK|wx.CANCEL)
        if dlg.ShowModal() == wx.ID_OK:
            self.filename = dlg.GetValue()
            # A new filename has been entered, but the data has not been read
            self.data_loaded = False
            # print "You entered: %s" % self.filename     
        dlg.Destroy()

    #  This starts with the long string of HTML to display
    class UsageFrame(wx.Frame):
        text = """
<HTML>
<HEAD></HEAD>
<BODY BGCOLOR="#D6E7F7">

<CENTER><H1>Using G-3 Netview</H1></CENTER>

<H2>Introduction and Quick Start</H2>

<p>Netview is a stand-alone Python application for viewing the output of
GENESIS 2 and 3 network simulations.  It is intended to replace GENESIS 2
SLI scripts that use the XODUS 'xview' widget.</p>

<p>The design and operation is based on the G3Plot application for creating 2D
plots of y(t) or y(x) from data files.  As with G3Plot, the main class
PlotFrame uses a basic wxPython frame to embed a matplotlib figure for
plotting.  It defines some basic menu items and a control panel of buttons
and toggles, each with bindings to a function to execute on a mouse click.</p>

<p>Unlike G3Plot, the image created with Netview is an animated
representation of a rectangular network with colored squares
used to indicate the value of some variable at that position
and time.  Typically, this would be the membrane potenial of
a cell soma, or a synaptic current in a dendrite segment.</p>

<h2>Usage</h2>

<p>The Menu Bar has <em>File/Open</em>, <em>File/Save</em>, and
<em>File/Exit</em> choices.  The Help Menu choices <em>About</em> and
<em>Usage</em> give further information.  The <em>Program Info</em>
selection shows code documentation that is contained in some of the main
function <em>docstrings</em>.</p>

<p>After starting the <em>netview</em> program, enter a data file name
in the dialog for File/Open, unless the filename was given as a
command line argument.  Then click on <strong>New Data</strong> to load the new
data and initialize the plot.  When the plot is cleared to black,
press <strong>Play</strong>.</p>

<p>The file types recognized are plain text or text files compressed with
bzip2.  The expected data format is one line for each output time step,
with each line having the membrane potential value of each cell in the net.
No time value should be given on the line.  In order to properly display
the data, netview needs some additional information about the network and
the data.  This can optionally be contained in a header line that precedes
the data.  If a header is not detected, a dialog will appear asking for the
needed parameters.</p>

<p>It is assumed that the cells are arranged on a NX x NY grid, numbered
from 0 (bottom left corner) to NX*NY - 1 (upper right corner).
In order to provide this information to netview, the data file should
begin with a header line of the form:</p>

<pre>
    #optional_RUNID_string Ntimes start_time dt NX NY SEP_X SEP_Y x0 y0 z0
</pre>

<p>The line must start with &quot;#&quot; and can optionally be followed immediately by any
string.  Typically this is some identification string generated by the
simulation run.  The following parameters, separated by blanks or any
whitespace, are:</p>

<ul>
<li>Ntimes - the number of lines in the file, exclusive of the header</li>
<li>start_time - the simulation time for the first data line (default 0.0)</li>
<li>dt - the time step used for output</li>
<li>NX, NY - the integer dimensions of the network</li>
<li>SEP_X, SEP_Y - the x,y distances between cells (optional)</li>
<li>x0, y0, z0 - the location of the compartment (data source) relative to the
cell origin</li>
</ul>

<p>The RUNID string and the last five parameters are not read or used
by netview.  These are available for other data analysis tools that
need a RUNID and the location of each source.</p>

<p>The slider bars can be used to set a time window for display, and the
<strong>Reset</strong> button can set t_min and t_max back to the defaults.
Use the <strong>Forward/Back</strong> toggle to reverse direction of
<strong>Play</strong>, and the <strong>Normal/Fast</strong> toggle to show
every tenth frame.</p> <p>The <strong>Single Step</strong> button can be
used to advance a single step at a time (or 10, if in 'Fast' mode).</p>

<p>The <strong>STOP</strong> button is currently not implemented</p>
<p>To plot different data, enter a new filename with <strong>File/Open</strong> and
repeat with <strong>New Data</strong> and <strong>Play</strong>.</p>

<HR>
</BODY>
</HTML>
        """

        def __init__(self, parent):
            wx.Frame.__init__(self, parent, -1, "Usage and Help",
                size=(640,600), pos=(400,100))
            html = wx.html.HtmlWindow(self)
            html.SetPage(self.text)
            panel = wx.Panel(self, -1)
            button = wx.Button(panel, wx.ID_OK, "Close")
            self.Bind(wx.EVT_BUTTON, self.OnCloseMe, button)
            sizer = wx.BoxSizer(wx.VERTICAL)
            sizer.Add(html, 1, wx.EXPAND|wx.ALL, 5)
            sizer.Add(panel, 0, wx.ALIGN_CENTER|wx.ALL, 5)
            self.SetSizer(sizer)
            self.Layout()

	def OnCloseMe(self, event):
            self.Close(True)

        # ----------- end of class UsageFrame ---------------

    def OnUsage(self,event):
        usagewin = self.UsageFrame(self)
        usagewin.Show(True)

    def OnInfo(self,event):
        msg = "Program information for PlotFrame obtained from docstrings:"
        msg += "\n" + self.__doc__ + "\n" + self.create_main_panel.__doc__
        msg += self.create_button_bar.__doc__
        msg += self.init_plot.__doc__
        msg += self.plot_data.__doc__
        dlg = wx.lib.dialogs.ScrolledMessageDialog(self, msg,
            "PlotFrame Documentation")
        dlg.ShowModal()

    #  ---------------------------------------------------------------
    #	Define the functions executed on control button click
    #  ---------------------------------------------------------------

    def OnRewind(self,event):
        self.get_data_params()
        self.init_plot()
        self.get_xyt_data()

    def OnReplot(self,event):
        self.plot_data()
        self.canvas.draw()

    def OnSstep(self,event):
        if self.data_loaded == False:
            # bring up a warning dialog
            msg = """
            Data for plotting has not been loaded!
            Please enter the file to plot with File/Open, unless
            it was already specified, and then click on 'New Data'
            to load the data to play back, before clicking 'Play'.
            """
            wx.MessageBox(msg, "Plot Warning", wx.OK | wx.ICON_ERROR,self)
            return

        self.t_max = min(self.t_max + self.dt, (self.Ntimes - 1)*self.dt)
        self.stmax.set_val(self.t_max)
        frame_num = int(self.t_max/self.dt)
        self.SetStatusText('time: '+str(frame_num*self.dt))
        self.im.set_data(self.ldata[frame_num])
        self.axes.draw_artist(self.im)
        self.canvas.blit(self.axes.bbox)

    def OnStop(self,event):
        self.STOP = 'True'

    def OnForward(self,event):
        state =	 self.forward_toggle.GetValue()
        if state:
            self.reverse_play = False
            self.forward_toggle.SetLabel("Forward ")
            self.forward_toggle.SetForegroundColour('black')
            self.forward_toggle.SetBackgroundColour('yellow')
        else:
            self.reverse_play = True
            self.forward_toggle.SetLabel("  Back  ")
            self.forward_toggle.SetForegroundColour('red')
            self.forward_toggle.SetBackgroundColour('green')

    def OnFast(self,event):
        state =	 self.fast_toggle.GetValue()
        if state:
            # print state
            self.fast_toggle.SetLabel(" Normal ")
            self.fast_toggle.SetForegroundColour('black')
            self.fast_toggle.SetBackgroundColour('yellow')
            self.step = 1
        else:
            # print state
            self.fast_toggle.SetLabel("  Fast  ")
            self.fast_toggle.SetForegroundColour('red')
            self.fast_toggle.SetBackgroundColour('green')
            self.step = 10

class MainApp(wx.App):
    def OnInit(self):
       frame = PlotFrame("G-3 Network View 1.7", (50, 60), (480, 640))
       frame.Show()
       self.SetTopWindow(frame)
       return True
# The main loop
                
if __name__ == '__main__':
    app = MainApp(False)
    app.MainLoop()
