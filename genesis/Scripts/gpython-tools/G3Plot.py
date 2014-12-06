#!/usr/bin/env python

# A prototype of a stand-alone application for plotting the output of
# GENESIS 3 models.  This uses a basic wxPython frame to hold a matplotlib
# figure for plotting.  It defines some basic menu items, and a control
# panel of buttons and toggles, each with bindings to a function to execute
# on a mouse click.  It is based on the example program wx_mpl_bars.py and
# other wxPython and matplotlib examples.

import sys, os, glob

# import needed wxPython modules
import wx
import wx.html
import  wx.lib.dialogs

import matplotlib
matplotlib.use('WXAgg')
from matplotlib.figure import Figure
from matplotlib.backends.backend_wxagg import \
    FigureCanvasWxAgg as FigCanvas, \
    NavigationToolbar2WxAgg as NavigationToolbar
import matplotlib.font_manager as font_manager

import numpy as np

class MainApp(wx.App):
    def OnInit(self):
       frame = PlotFrame("GENESIS 3 Plot Demo", (50, 60), (640, 640))
       frame.Show()
       self.SetTopWindow(frame)
       return True
    
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
    def __init__(self, title, pos, size):
        wx.Frame.__init__(self, None, wx.ID_ANY, title, pos, size)
        # define some variables to be shared among functions below
        # default filename list will be either empty or from args
        self.filenames = sys.argv[1:]
        # python 2.5 and later hack to remove duplicates: list to set to list
        self.filenames = list(set(self.filenames))
        self.plot_type = 'generic'
        # format string for plot - default is color black
        self.plot_format = 'k'

        #    Make the main Matplotlib panel for plots
        self.create_main_panel()  # creates canvas

        #
        # Layout with box sizers
        #
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
        #              set up the Menu Bar
        # -------------------------------------------------------
        menuBar = wx.MenuBar()

        menuFile = wx.Menu() # File menu
        menuFile.Append(1, "&Open", "Filename(s) or wildcard list to plot")
        menuFile.Append(3, "Save", "Save plot as a PNG image")
        menuFile.AppendSeparator()
        menuFile.Append(10, "E&xit")
        menuBar.Append(menuFile, "&File")

        menuHelp = wx.Menu() # Help menu
        menuHelp.Append(11, "&About Basic Plot")
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
        self.set_plot_type()

        # ---------- end of __init__ ----------------------------

    # -------------------------------------------------------
    #   Function to make the main Matplotlib panel for plots
    # -------------------------------------------------------
    def create_main_panel(self):
        """ 
        create_main_panel creates the main panel with:
                *  mathplotlib canvas 
                *  mathplotlib navigation toolbar
        """
        self.panel = wx.Panel(self)
        
        # Create the mpl Figure and FigCanvas objects. 
        # 5x4 inches, 100 dots-per-inch
        #
        self.dpi = 100
        self.fig = Figure((5.0, 4.0), dpi=self.dpi)
        self.canvas = FigCanvas(self.panel, wx.ID_ANY, self.fig)
        
        # Since we have only one plot, we could use add_axes 
        # instead of add_subplot, but then the subplot
        # configuration tool in the navigation toolbar wouldn't work.
        #
        # (111) == (1,1,1) --> row 1, col 1, Figure 1)
        self.axes = self.fig.add_subplot(111)
        #
        # Create the navigation toolbar, tied to the canvas
        #
        self.toolbar = NavigationToolbar(self.canvas)

    def create_button_bar(self):
        """
        create_button_bar makes a control panel bar with buttons and
        toggles for

        Clear - Plot - Overlay ON/OFF - generic/Vm plot - Legend OFF/On
    
        It does not create a Panel container, but simply creates Button
        objects with bindings, and adds them to a horizontal BoxSizer
        self.button_bar_sizer.  This is added to the PlotFrame vertical
        BoxSizer during initialization of the frame.

        """
        clear_button = wx.Button(self.panel, -1, "Clear")
        self.Bind(wx.EVT_BUTTON, self.OnClear, clear_button)

        replot_button = wx.Button(self.panel, -1, "Plot")
        self.Bind(wx.EVT_BUTTON, self.OnReplot, replot_button)

        # The toggle buttons need to be globally accessible
        self.overlay_button = wx.ToggleButton(self.panel, -1, " Overlay ON ")
        self.overlay_button.SetValue(True)
        # the default is self.axes.hold(True)
        self.overlay_button.SetLabel(" Overlay ON ")
        self.Bind(wx.EVT_TOGGLEBUTTON, self.OnOverlay, self.overlay_button)

        self.autoscale_button = wx.ToggleButton(self.panel, -1, " generic plot ")
        self.autoscale_button.SetValue(False)
        self.autoscale_button.SetLabel(" generic plot ")
        self.Bind(wx.EVT_TOGGLEBUTTON, self.OnAutoscale, self.autoscale_button)

        self.legend_button = wx.ToggleButton(self.panel, -1, " Legend OFF ")
        self.legend_button.SetValue(False)
        self.legend_button.SetLabel(" Legend OFF ")
        self.Bind(wx.EVT_TOGGLEBUTTON, self.OnLegend, self.legend_button)

        # info_button = wx.Button(self.panel, -1, "Info")
        # self.Bind(wx.EVT_BUTTON, self.OnInfo, info_button)

        # Set button colors to match G2 "colorize function" defaults
        # This is highly dependent on X11 color definitions
        clear_button.SetBackgroundColour('rosybrown1')
        replot_button.SetBackgroundColour('rosybrown1')
        self.overlay_button.SetForegroundColour('red')
        self.overlay_button.SetBackgroundColour('cadetblue1')
        self.autoscale_button.SetForegroundColour('blue')
        self.autoscale_button.SetBackgroundColour('cadetblue1')
        self.legend_button.SetForegroundColour('blue')
        self.legend_button.SetBackgroundColour('cadetblue1')

        self.button_bar_sizer = wx.BoxSizer(wx.HORIZONTAL)
        flags = wx.ALIGN_CENTER | wx.ALL
        self.button_bar_sizer.Add(clear_button, 0, border=3, flag=flags)
        self.button_bar_sizer.Add(replot_button, 0, border=3, flag=flags)
        self.button_bar_sizer.Add(self.overlay_button, 0, border=3, flag=flags)
        self.button_bar_sizer.Add(self.autoscale_button, 0, border=3, flag=flags)
        self.button_bar_sizer.Add(self.legend_button, 0, border=3, flag=flags)

    # -------------------------------------------------------
    #   Functions to generate or read (x,y) data and plot it
    # -------------------------------------------------------

    def plot_file(self):
        # print 'Plotting %s' % self.file
        x = []; y = []
        fp = open(self.file, 'r')
        # print 'Opened %s' % self.file
        for line in fp.readlines():
            data = line.split(" ")
            x.append(data[0]); y.append(data[1])
        self.axes.plot(x, y, self.plot_format)

    def plot_data_files(self):
        formats = ['k', 'r', 'b', 'g', 'm', 'c']
        plotnum = 0
        if len(self.filenames) > 0:
            for self.file in self.filenames:
                self.plot_format = formats[plotnum % len(formats)]
                try:
                    if os.path.exists(self.file):
                        self.plot_file()
                        plotnum += 1
                    else:
                        print '*** Error: Incorrect file name or path specified'
                # I need to do better error handling!
                except:
                    print 'An error ocurred'
                    # sys.exit()
        else:
            # bring up a warning dialog
                msg = """
                No existing files were specified for plotting!

                Please enter one or more files to plot with File/Open.
                """
                wx.MessageBox(msg, "Plot Warning", wx.OK | wx.ICON_ERROR,self)


    #  ---------------------------------------------------------------        
    #   Define the functions executed on menu choices
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
            self.flash_status_message("Saved to %s" % path)

    def OnOpen(self, event):
        dlg = wx.TextEntryDialog(self, "File(s) with x,y data to plot",
            "File Open", "Vm.out", style=wx.OK|wx.CANCEL)
        if dlg.ShowModal() == wx.ID_OK:
            filename_string = dlg.GetValue()
            # print "You entered: %s" % filename_string     
            # expand wildcarded file list in filename_string
            # For now, replace and not add to filenames - RadioButtons later?
            self.filenames = []
            if filename_string != "":
                for name in filename_string.split():
                    self.filenames += glob.glob(name)
            files_plotted = ""
            # python 2.5 and later remove duplicates: list to set to list
            self.filenames = list(set(self.filenames))
            for name in self.filenames:
                files_plotted += " " + name
            self.SetStatusText("Plotting " + files_plotted)
            # print "filenames = ", self.filenames
        dlg.Destroy()

    def OnAbout(self, event):
        msg = """
                       G3Plot ver. 0.5

G3Plot is a protoype of a stand-alone application for
plotting the output of GENESIS 3 simulations. It defines a
custom wxPython frame class PlotFrame to embed a matplotlib
figure for plotting. PlotFrame defines some basic menu items
on the menu bar, and a control panel of colored buttons and
toggles, with bindings to functions executed on a mouse
click.  The functions for the Help menu choices provide
examples of providing documentation through the wxPython
classes MessageDialog, ScrolledMessageDialog, Dialog, and
HtmlWindow, for HTML-formatted documentation.

Help/Usage gives HTML help for using G3Plot to plot data
files.  This is the main Help page.

Help/Program Info provides some information about the
objects and functions, and the wxPython and matplotlib
classes used here.

Dave Beeman, April 2010
        """
        dlg = wx.MessageDialog(self, msg, "About G3 Plot and PlotFrame",
            wx.OK | wx.ICON_QUESTION)
	dlg.ShowModal()
	dlg.Destroy()

    class UsageFrame(wx.Frame):
        text = """
<HTML>
<HEAD></HEAD>
<BODY BGCOLOR="#D6E7F7">

<CENTER><H1>Using G3 Plot and PlotFrame</H1></CENTER>

<H2>Introduction and Quick Start</H2>

<P> PlotFrame is a prototype of a Python class for use in an application
for plotting the output of GENESIS 3 simulations. It uses a basic wxPython
frame to embed a matplotlib figure for plotting.  It defines some basic
menu items and a control panel of buttons and toggles, each with bindings
to a function to execute on a mouse click, and can aso be used as an
example of embedding Matplotlib plots within a wx Widgets environment.

<P>

G3Plot is used to plot multiple files, with a number of features designed
to simplify the process of comparing multiple plots of membrane potential.
You may specify a list of one or more files that each contain multiple
lines of (x,y) data pairs separated by white space.  The file list may
contain wildards, and can be specified either on the command line, e.g.:

<PRE>
      G3Plot Vm.out Vm_data/pyr*.out
</PRE>

or in the dialog brought up from the File/Open menu.  The wildcards will be
expanded and paths to the matching files will go into a <i>filenames</i>
list, which has any duplicate wildcard matches removed.

Then, click on the Plot button in the Control Button Bar to plot the data
in the file or files in the <i>filenames</i> list.

<H2>Menu Bar Choices</H2>

<UL>
  <LI><B>File</B>
  <UL>
    <LI><B>Open</B> - Enter the filename(s) or wildcarded list of files to
    plot.

    <LI><B>Save</B> - Set filename, browse for directory, and save the
	   current plot in a PNG format file.

    <LI><B>Exit</B>  - Exit the program

  </UL>
  <LI><B>Help</B>

  <UL>
    <LI><B>About</B> - Brief description of this application

    <LI><B>Usage</B> - Main Help and Usage Information

    <LI><B>Program Info</B> - Some details of the implementation, obtained
           from internal documentation strings

  </UL>
</UL>
<H2>Control Button Bar</H2>

<UL>
  <LI><B>[Clear]</B> - Clear the plot

  <LI><B>[Plot]</B> - Plot the data in the file(s) that are in the
      <i>filenames</i> list

  <LI><B>[Overlay ON/OFF]</B> - When ON (the default), multiple files in the
  <i>filenames</i> list will be plotted on the same axes without clearing
  the plot.  When OFF, the plot is cleared after each file is plotted.

  <LI><B>[generic/Vm plot]</B> - When set to 'generic plot' , the title and
  axis labels are set for a generic (x,y) plot.  When set to 'Vm plot', the
  title and axis labels are set to values appropriate for membrane
  potential (volts) vs time (seconds).

  <LI><B>[Legend ON/OFF]</B> - Toggle the plot legend on and off.  The
  legend is made from the <i>filenames</i> list, using the colors of the
  corresponding line plot.  The legend will not be displayed if the
  currently displayed plot does not have the same number of line plots as
  there are files in the list, e.g. it will not attempt to show the legend
  after clearing the plot, and will pop up a warning dialog.

</UL>
<H2>Matplotlib Navigation Panel</H2>

<P>
Navigation within the current plot is performed using the seven icons
displayed in the panel below the plot.

<P>
The Home, Back, and Forward buttons are used to navigate back and forth
between previously defined views.  You can create a new view of your data
by using the Pan or Zoom buttons.

<UL>
  <LI><B>[ |^| ] Home</B>  - Go to the first, default view

  <LI><B>[ &lt;-- ] Back</B> - Go to previous view, if one exists

  <LI><B>[ --&gt; ] Forward</B> - Go to next view, if one exists

  <LI><B>[ + ] Pan</B> - When this button is activated, click left mouse
  button and drag to pan the figure, dragging it to a new position.

  <LI><B>[ O ] Zoom</B> - When this button is activated, click left mouse
  button and drag to define a zoom region.

  <LI><B>[  #  ] Configure Subplots</B> - configure the parameters of the plot: the
 left, right, top, bottom, space between the rows and space between
 the columns

  <LI><B>[ disk ] Save</B> - Launch a file save dialog for the plot

</UL>
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

    def OnUsage(self,event):
        usagewin = self.UsageFrame(self)
        usagewin.Show(True)

    def OnInfo(self,event):
        msg = "Program information for PlotFrame obtained from docstrings:"
        msg += "\n" + self.__doc__ + "\n" + self.create_main_panel.__doc__
        msg += self.create_button_bar.__doc__
        dlg = wx.lib.dialogs.ScrolledMessageDialog(self, msg,
            "PlotFrame Documentation")
	dlg.ShowModal()

    #  ---------------------------------------------------------------        
    #   Define the functions executed on control button click
    #  ---------------------------------------------------------------        
    def OnClear(self,event):
        self.axes.clear()
        self.canvas.draw()
        # self.Refresh()

    def OnReplot(self,event):
        self.plot_data_files()
        self.canvas.draw()

    def OnOverlay(self,event):
        state =  self.overlay_button.GetValue()
        if state:
            # print state
            self.overlay_button.SetLabel("Overlay ON")
            self.overlay_button.SetForegroundColour('red')
            self.axes.hold(True)
        else:
            # print state
            self.overlay_button.SetLabel("Overlay OFF")
            self.overlay_button.SetForegroundColour('blue')
            self.axes.hold(False)

    def OnAutoscale(self,event):
	state =	 self.autoscale_button.GetValue()
        if state:
            self.autoscale_button.SetLabel("   Vm plot  ")
            self.autoscale_button.SetForegroundColour('red')
            self.plot_type = 'Vm'
        else:
            self.autoscale_button.SetLabel("generic plot")
            self.autoscale_button.SetForegroundColour('blue')
            self.plot_type = 'generic'
        self.set_plot_type()

    def set_plot_type(self):
        # print self.plot_type
        if self.plot_type == 'generic':
            self.axes.set_autoscale_on(True)
            self.axes.set_title('(x,y) data')
            self.axes.set_xlabel('X')
            self.axes.set_ylabel('Y')
            self.canvas.draw()
            # self.Refresh()
        else:
            self.axes.set_autoscale_on(False)

            self.axes.set_title('Membrane Potential')
            self.axes.set_xlabel('seconds')
            self.axes.set_ylabel('Volts')
            self.axes.axis(ymin=-0.1, ymax=0.05)
            self.canvas.draw()
            # self.Refresh()

    def OnLegend(self,event):
        state =  self.legend_button.GetValue()
        if state:
            self.legend_button.SetLabel("Legend ON")
            self.legend_button.SetForegroundColour('red')
            # Need to check that number in list == number of lines on plot
            # Could still be problems if plot is cleared and no filenames
            nlines = len(self.axes.get_lines())
            if (len(self.filenames) == nlines) and (nlines != 0):
                # use filenames list for legend and reduce font size
                self.axes.legend(self.filenames, 
                    prop=font_manager.FontProperties(size=10))
            else:
                msg = "The plot has " + str(len(self.axes.get_lines())) + \
                " lines, but the filenames list has " + \
                str(len(self.filenames)) + " entries.\n\n" + \
                "Please enter one or more files to plot with File/Open, " + \
                " click on Clear, and then Plot.  Then toggle to Legend ON."
                wx.MessageBox(msg, "Legend Warning", wx.OK | wx.ICON_ERROR, self)
            self.canvas.draw()
        else:
            self.legend_button.SetLabel("Legend OFF")
            self.legend_button.SetForegroundColour('blue')
            self.autoscale_button.SetForegroundColour('blue')
            ax = self.fig.gca()
            ax.legend_ = None
            self.canvas.draw()

    #  ---------------------------------------------------------------        
    #   Define some auxillary functions
    #  ---------------------------------------------------------------        
    def flash_status_message(self, msg, flash_len_ms=1500):
        self.statusbar.SetStatusText(msg)
        self.timeroff = wx.Timer(self)
        self.Bind(
            wx.EVT_TIMER, 
            self.on_flash_status_off, 
            self.timeroff)
        self.timeroff.Start(flash_len_ms, oneShot=True)
    
    def on_flash_status_off(self, event):
        self.statusbar.SetStatusText('')
                
if __name__ == '__main__':
    app = MainApp(False)
    app.MainLoop()
