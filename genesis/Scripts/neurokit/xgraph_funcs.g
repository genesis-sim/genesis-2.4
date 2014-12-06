//genesis

function expscale(factor, form)
	float factor
	str form

	str field
	float value

	field = {getfield {form}/field value}
	value = {getfield {form}/{field} value}

	value = value*factor
	setfield {form}/{field} value {value}
end

function linscale(factor, form)
	float factor
	str form

	str field
	float value
	float v1, v2
	float r
	float l, t
	int il, it
	float corr

	field = {getfield {form}/field value}
	value = {getfield {form}/{field} value}

	r = value
	r = {abs {r}}
	l = {log {r}}/{log 10}
	if (l < 0)
		il = l - 1
		it = {pow 10 {-il}}
	else
		il = l
		it = {pow 10 {il}}
	end
	t = il
	t = {pow 10 {il}}
	l = r/t
	if (l > 9.999)
		t = t*10.0
		l = l/10.0
		if (il < 0)
			it = it/10
		else
			it = it*10
		end
		il = il + 1
	end
	//	echo l = {l}, il = {il},  r = {r}, t = {t}
	if ((l >= 0.999) && (l < 2))
		r = t*0.1
	end
	if ((l >= 1.999) && (l < 5))
		r = t*0.2
	end
	if (l >= 4.999)
		r = t*0.5
	end
	if (value > 0)
		corr = 0.5
	else
		corr = -0.5
	end
	t = it
	if (il > 0)
		value = {trunc {((value + r*factor)*10.0/t + corr)}}*t/10.0
	else
		value = {trunc {((value + r*factor)*t*10.0 + corr)}}/(t*10.0)
	end
	setfield {form}/{field} value {value}
end

function overscale(graph, widget)
	str graph, widget
	if ({getfield {widget} state} == 1)
		setfield {graph} overlay 1
	else
		setfield {graph} overlay 0
	end
end

function scalegraph(graph)
	str graph

	int x, y, w
	str temp

	//DHB temp = {getfield {graph} form}
	//DHB x = {getfield {temp} x}
	//DHB y = {getfield {temp} y}
	x = {getfield {graph}/.. xgeom}
	y = {getfield {graph}/.. ygeom}

	create x1form {graph}_scale [{x},{y},200,280] -title Scale
	create x1button {graph}/scale [0,0,50,] -script  \
	    "xshowontop "{graph}_scale
	create x1toggle {graph}/overlay [170,0,120,] -script  \
	    "overscale "{graph}" "<widget>
	setfield ^ label0 "do not overlay" label1 "overlay"

	pushe {graph}_scale

	create x1button "<<" [2%,35,20%,] -script  \
	    "expscale 0.5 "{graph}_scale
	create x1button "<" [24%,35,20%,] -script  \
	    "linscale -1.0 "{graph}_scale
	create x1button " >" [56%,35,20%,] -script  \
	    "linscale 1.0 "{graph}_scale
	create x1button " >>" [78%,35,20%,] -script  \
	    "expscale 2.0 "{graph}_scale
	create x1dialog field -value xmin -script "setwidgfield "<widget>

	create x1dialog xmin -value {getfield {graph} xmin}
	create x1dialog xmax -value {getfield {graph} xmax}
	create x1dialog ymin -value {getfield {graph} ymin}
	create x1dialog ymax -value {getfield {graph} ymax}
	create x1button APPLY -script  \
	    "applyscale "{graph}" "{graph}_scale" 0"
	create x1button APPLY_AND_VANISH -script  \
	    "applyscale "{graph}" "{graph}_scale" 1"

	pope
end

function setwidgfield(widget)
	str widget

	str field

	field = {getfield {widget} value}
	if ({strcmp {field} xmin} == 0)
		setfield {widget} value xmax
	end
	if ({strcmp {field} xmax} == 0)
		setfield {widget} value ymin
	end
	if ({strcmp {field} ymin} == 0)
		setfield {widget} value ymax
	end
	if ({strcmp {field} ymax} == 0)
		setfield {widget} value xmin
	end
end

function applyscale(graph, form, vanish)
	str graph, form
	int vanish

	float xmin, ymin, xmax, ymax

	xmin = {getfield {form}/xmin value}
	xmax = {getfield {form}/xmax value}
	ymin = {getfield {form}/ymin value}
	ymax = {getfield {form}/ymax value}

	if (xmin > xmax)
		setfield {form}/xmin value {xmax}
		setfield {form}/xmax value {xmin}
		xmin = {getfield {form}/xmin value}
		xmax = {getfield {form}/xmax value}
	end

	if (ymin > ymax)
		setfield {form}/ymin value {ymax}
		setfield {form}/ymax value {ymin}
		ymin = {getfield {form}/ymin value}
		ymax = {getfield {form}/ymax value}
	end

	if (xmin != {getfield {graph} xmin})
		setfield {graph} xmin {xmin}
	end

	if (xmax != {getfield {graph} xmax})
		setfield {graph} xmax {xmax}
	end

	if (ymin != {getfield {graph} ymin})
		setfield {graph} ymin {ymin}
	end

	if (ymax != {getfield {graph} ymax})
		setfield {graph} ymax {ymax}
	end

	/*
	set {graph} xmin {xmin} ymin {ymin} xmax {xmax} ymax {ymax}
	*/

	if (vanish)
		xhide {form}
	end
end

function do_xgraph(num)
	str num

	int xoutwidth = (user_screenwidth - 380)/2

	if ({strcmp {num} "1"} == 0)
		if ((user_numxouts) == 1)
			create x1form {cellpath}graph1 [380,540,600,350]  \
			    -nolabel
		else
			create x1form {cellpath}graph1 [380,540,450,350]  \
			    -nolabel
		end
	else
		create x1form {cellpath}graph2  \
		    [{380 + xoutwidth},540,{xoutwidth},350] -nolabel
	end
	pushe {cellpath}graph{num}
	disable .
	create x1graph graph [1%,5%,98%,95%] -range [0,-0.1,0.1,0.4]
	setfield ^ XUnits Sec
	if ({strcmp {num} "1"} == 0)
		setfield graph yoffset {user_yoffset1} xmax {user_xmax1} \
		     ymin {user_ymin1} ymax {user_ymax1}
	else
		setfield graph yoffset {user_yoffset2} xmax {user_xmax2} \
		     ymin {user_ymin2} ymax {user_ymax2}
	end
	useclock graph 9
	scalegraph {cellpath}graph{num}/graph
	create x1button {cellpath}graph{num}/graph/new_colors  \
	    [70,0,80,] -script "color_plot "{num}
	pope
end
