//genesis

if (({exists /xproto}))
	return
end


xcolorscale hot
xps -mingray 255 -maxgray 256 -inverse 0
create x1form /xproto [20,20,800,400]

/* These colors are set up for Sun 386 scale to make what I think are
the correct colors. Someone at Sun is colorblind. Try pink, and see. */
str White = "white"
str Black = "black"
str Red = "red"
str Orange = "orange"
str Yellow = "yellow"
str Green = "green"
str Blue = "blue"
str Violet = "BlueViolet"
str Purple = "orchid"

create x1draw /xproto/draw
ce /xproto/draw
create x1shape fillbox.icon -coords  \
    [-1.000000,-1.000000,0.000000] \
    [1.000000,-1.000000,0.000000] \
    [1.000000,1.000000,0.000000] \
    [-1.000000,1.000000,0.000000]
create x1shape fillbox.name

create x1shape circle.icon -coords  \
    [0.000000,1.000000,0.000000] [0.309007,0.951060,0.000000] \
    [0.587769,0.809029,0.000000] [0.808999,0.587810,0.000000] \
    [0.951044,0.309055,0.000000] [1.000000,0.000050,0.000000] \
    [0.951075,-0.308960,0.000000] \
    [0.809058,-0.587729,0.000000][0.587850,-0.808970,0.000000] \
    [0.309103,-0.951029,0.000000][0.000100,-1.000000,0.000000] \
    [-0.308912,-0.951090,0.000000][-0.587688,-0.809088,0.000000] \
    [-0.808941,-0.587890,0.000000][-0.951013,-0.309150,0.000000] \
    [-1.000000,-0.000150,0.000000][-0.951106,0.308865,0.000000] \
    [-0.809117,0.587648,0.000000][-0.587931,0.808911,0.000000] \
    [-0.309198,0.950998,0.000000][0.000000,1.000000,0.000000]
create x1shape circle.name

create x1shape soma.icon -coords  \
    [0.000000,1.000000,0.000000][0.309007,0.951060,0.000000] \
    [0.587769,0.809029,0.000000][0.808999,0.587810,0.000000] \
    [0.951044,0.309055,0.000000][1.000000,0.000050,0.000000] \
    [0.951075,-0.308960,0.000000][0.809058,-0.587729,0.000000] \
    [0.587850,-0.808970,0.000000][0.309103,-0.951029,0.000000] \
    [0.000100,-1.000000,0.000000][-0.308912,-0.951090,0.000000] \
    [-0.587688,-0.809088,0.000000][-0.808941,-0.587890,0.000000] \
    [-0.951013,-0.309150,0.000000][-1.000000,-0.000150,0.000000] \
    [-0.951106,0.308865,0.000000][-0.809117,0.587648,0.000000] \
    [-0.587931,0.808911,0.000000][-0.309198,0.950998,0.000000] \
    [0.000000,1.000000,0.000000]
create x1shape soma.name


create x1shape compartment.icon -coords  \
    [0.600000,0.800000,0.000000][0.900000,0.600000,0.000000] \
    [1.000000,0.400000,0.000000][1.000000,0.200000,0.000000] \
    [0.800000,-0.200000,0.000000][-0.600000,-0.400000,0.000000] \
    [-0.400000,0.000000,0.000000][-0.400000,0.200000,0.000000] \
    [-0.500000,0.400000,0.000000][-0.800000,0.600000,0.000000] \
    [0.600000,0.800000,0.000000][-0.800000,0.600000,0.000000] \
    [-1.000000,0.200000,0.000000][-1.000000,0.000000,0.000000] \
    [-0.900000,-0.200000,0.000000][-0.600000,-0.400000,0.000000]
create x1shape compartment.name


create x1shape box.icon -coords  \
    [-1.000000,-1.000000,0.000000][1.000000,-1.000000,0.000000] \
    [1.000000,1.000000,0.000000][-1.000000,1.000000,0.000000] \
    [-1.000000,-1.000000,0.000000]
create x1shape box.name


create x1shape buffer.icon -coords  \
    [-1.000000,-1.000000,0.000000][1.000000,-1.000000,0.000000] \
    [1.000000,1.000000,0.000000][-1.000000,1.000000,0.000000]
create x1shape buffer.name


create x1shape axon.icon -coords  \
    [-1.000000,0.800000,0.000000][0.500000,0.500000,0.000000] \
    [0.000000,1.000000,0.000000][1.000000,0.000000,0.000000] \
    [0.000000,-1.000000,0.000000][0.500000,-0.500000,0.000000] \
    [-1.000000,-0.800000,0.000000]
create x1shape axon.name


create x1shape hhna.icon -coords  \
    [0.200000,0.600000,0.000000][0.800000,0.600000,0.000000] \
    [1.000000,0.400000,0.000000][1.000000,-1.000000,0.000000] \
    [0.400000,-1.000000,0.000000][0.200000,-0.800000,0.000000] \
    [0.200000,0.600000,0.000000][0.400000,0.700000,0.000000] \
    [0.400000,0.900000,0.000000][0.200000,1.000000,0.000000] \
    [-0.600000,1.000000,0.000000][-1.000000,0.600000,0.000000] \
    [-1.000000,-1.000000,0.000000][-0.200000,-1.000000,0.000000] \
    [-0.200000,0.500000,0.000000][0.200000,0.600000,0.000000]

create x1shape hhna.name


create x1shape hhk.icon -coords  \
    [0.200000,0.600000,0.000000][0.800000,0.600000,0.000000] \
    [1.000000,0.400000,0.000000][1.000000,-1.000000,0.000000] \
    [0.400000,-1.000000,0.000000][0.200000,-0.800000,0.000000] \
    [0.200000,0.600000,0.000000][0.400000,0.700000,0.000000] \
    [0.400000,0.900000,0.000000][0.200000,1.000000,0.000000] \
    [-0.600000,1.000000,0.000000][-1.000000,0.600000,0.000000] \
    [-1.000000,-1.000000,0.000000][-0.200000,-1.000000,0.000000] \
    [-0.200000,0.500000,0.000000][0.200000,0.600000,0.000000]
create x1shape hhk.name


create x1shape excsyn.icon -coords  \
    [-1.000000,0.100000,0.000000][0.000000,0.100000,0.000000] \
    [0.400000,0.400000,0.000000][1.000000,0.900000,0.000000] \
    [1.000000,-0.900000,0.000000][0.400000,-0.400000,0.000000] \
    [0.000000,-0.10000,0.000000][-1.000000,-0.100000,0.000000]
create x1shape excsyn.name


create x1shape inhsyn.icon -coords  \
    [-1.000000,0.050000,0.000000][0.000000,0.050000,0.000000] \
    [0.200000,0.200000,0.000000][1.000000,0.600000,0.000000] \
    [1.000000,-0.600000,0.000000][0.200000,-0.200000,0.000000] \
    [0.000000,-0.050000,0.000000][-1.000000,-0.050000,0.000000]
create x1shape inhsyn.name


create x1shape plane.icon -coords  \
    [-1.000000,-0.600000,0.000000][-0.600000,0.600000,0.000000] \
    [1.000000,0.600000,0.000000][0.600000,-0.600000,0.000000] \
    [-1.000000,-0.600000,0.000000]
create x1shape plane.name

create x1shape widget.icon -coords  \
    [-1,1,0][1,1,0][1,-1,0][-1,-1,0][-1,1,0][-1,-0.2,0][1,-0.2,0] \
    [1,0.4,0][0.5,0.3,0][0,0.9,0][-0.4,0.6,0][-1,0,0][-1,-0.6,0] \
    [-0.2,-0.6,0][-0.2,-0.9,0][-1,-0.9,0][-1,-1,0][1,-1,0][1,-0.9,0] \
    [0,-0.9,0][0,-0.6,0][1,-0.6,0]
create x1shape widget.name

create x1shape trunc.icon -coords  \
    [-0.2,-1,0][-0.2,-0.6,0][-1,-0.6,0][-0.2,-0.2,0][-0.8,-0.2,0] \
    [-0.2,0.2,0][-0.6,0.2,0][-0.2,0.6,0][-0.4,0.6,0][0,1,0][0,-1,0]
create x1shape trunc.name

create x1shape tree.icon -coords  \
    [-0.2,-1,0][-0.2,-0.6,0][-1,-0.6,0][-0.2,-0.2,0][-0.8,-0.2,0] \
    [-0.2,0.2,0][-0.6,0.2,0][-0.2,0.6,0][-0.4,0.6,0][0,1,0][0.4,0.6,0] \
    [0.2,0.6,0][0.6,0.2,0][0.2,0.2,0][0.8,-0.2,0][0.2,-0.2,0][-1,-0.6,0] \
    [0.2,-0.6,0][0.2,-1,0]
create x1shape tree.name

create x1shape ydisc.icon -coords  \
    [0.000000,0.0,1.000000][0.309007,0.0,0.951060] \
    [0.587769,0.0,0.809029][0.808999,0.0,0.587810] \
    [0.951044,0.0,0.309055][1.000000,0.0,0.000050] \
    [0.951075,0.0,-0.308960][0.809058,0.0,-0.587729] \
    [0.587850,0.0,-0.808970][0.309103,0.0,-0.951029] \
    [0.000100,0.0,-1.000000][-0.308912,0.0,-0.951090] \
    [-0.587688,0.0,-0.809088][-0.808941,0.0,-0.587890] \
    [-0.951013,0.0,-0.309150][-1.000000,0.0,-0.000150] \
    [-0.951106,0.0,0.308865][-0.809117,0.0,0.587648] \
    [-0.587931,0.0,0.808911][-0.309198,0.0,0.950998] \
    [0.000000,0.0,1.000000]
create x1shape ydisc.name


create x1shape xdisc.icon -coords  \
    [0.000000,0.0,1.000000][0.0,0.309007,0.951060] \
    [0.0,0.587769,0.809029][0.0,0.808999,0.587810] \
    [0.0,0.951044,0.309055][0.0,1.000000,0.000050] \
    [0.0,0.951075,-0.308960][0.0,0.809058,-0.587729] \
    [0.0,0.587850,-0.808970][0.0,0.309103,-0.951029] \
    [0.000100,0.0,-1.000000][0.0,-0.308912,-0.951090] \
    [0.0,-0.587688,-0.809088][0.0,-0.808941,-0.587890] \
    [0.0,-0.951013,-0.309150][0.0,-1.000000,-0.000150] \
    [0.0,-0.951106,0.308865][0.0,-0.809117,0.587648] \
    [0.0,-0.587931,0.808911][0.0,-0.309198,0.950998] \
    [0.000000,0.0,1.000000]
create x1shape xdisc.name

copy excsyn.icon channelA.icon
copy excsyn.icon channelB.icon
copy excsyn.icon channelC.icon
copy excsyn.icon channelC2.icon
copy excsyn.icon synchan.icon
copy excsyn.icon manuelconduct.icon
copy box.icon Ca_concen.icon
copy box.icon diffamp.icon
copy box.icon leakage.icon
copy compartment.icon symcompartment.icon
copy box.icon unit.icon
copy box.icon site.icon
copy buffer.icon graded.icon
copy buffer.icon linear.icon
copy buffer.icon periodic.icon
copy buffer.icon playback.icon
copy buffer.icon random.icon
copy compartment.icon RC.icon
copy buffer.icon sigmoid.icon
copy buffer.icon spike.icon
copy buffer.icon spikegen.icon
copy excsyn.icon synapse.icon
copy excsyn.icon synapseA.icon
copy excsyn.icon synapseB.icon
copy axon.icon axonlink.icon

copy hhna.icon hh_channel.icon
copy hhk.icon vdep_gate.icon
copy hhna.icon vdep_channel.icon

copy excsyn.icon receptor.icon
copy excsyn.icon receptor2.icon
copy excsyn.icon ddsyn.icon
copy excsyn.icon hebbsynchan.icon
copy hhk.icon tabchannel.icon
copy hhk.icon tab2Dchannel.icon
copy hhk.icon tabcurrent.icon
copy box.icon Mg_block.icon
copy box.icon nernst.icon
copy box.icon poolbuffer.icon
copy box.icon difpool.icon
copy box.icon disk_in.icon
copy box.icon hsolve.icon

copy box.icon ghk.icon
copy box.icon concpool.icon
copy box.icon difbuffer.icon
copy box.icon dif2buffer.icon
copy box.icon difshell.icon
copy box.icon fixbuffer.icon
copy box.icon fura2.icon
copy box.icon mmpump.icon

copy widget.icon xform.icon
copy widget.icon xlabel.icon
copy widget.icon xbutton.icon
copy widget.icon xdialog.icon
copy widget.icon xtoggle.icon
copy widget.icon xgraph.icon
copy widget.icon xdraw.icon
copy widget.icon xview.icon
copy widget.icon xconn.icon
copy tree.icon xelmtree.icon
copy widget.icon xshape.icon
copy widget.icon xfileview.icon

ce /
