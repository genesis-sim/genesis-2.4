// This illustrates farm mode of operation using barriers and synchrnous
// rpc to control master-worker work distribution and termination.

int a, b, c, d, flag, sum, mymatch

function setpars(ax,bx,cx,dx)
  int ax, bx, cx, dx
  a = ax
  b = bx
  c = cx
  d = dx
end

function setflag
  flag = 1
end

function runsim
  echo@0.0 {node}: runsim {a} {b} {c} {d}
  step 10
  mymatch = {node}
end

function report_match(nd, val)
	int nd,val
  int i = val/nd
  echo@0.0 {nd-1}: reporting
  sum = sum + i			// should increment sum
end

