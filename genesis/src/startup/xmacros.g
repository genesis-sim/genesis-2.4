
macro xsetview
	if ( $# < 7 )
		echo usage : xsetview planename element item itemname field itemtype 
		echo [display_mode color display_number clock]
		EndScript
	endif
	add_output $1 ( $3 )/( $6 ) @( $2 ):( $4 ) $5 
	set output name $1
	set output type xview_type
	set output output_func XviewOutput
	if ( $#  > 7 )
		if ( &strcmp( box , $7 ) == 0 ) 
			set output .display_mode 0
		endif
		if ( &strcmp( cross , $7 ) == 0 ) 
			set output .display_mode 1
		endif
		if ( &strcmp( fillbox , $7 ) == 0 ) 
			set output .display_mode 2
		endif
		if ( &strcmp( color , $7 ) == 0 )
			set output .display_mode 3
		endif
		if ( &strcmp( dot , $7 ) == 0 )
			set output .display_mode 4
		endif
	endif
	if ( $#  > 8 )
		set output .color	$8
	endif
	if ( $#  > 9 )
		set output .plotnum $9
	endif
	if ( $#  > 10 )
		set output clock $10
	endif
endmacro

/*
usage : xsetgraph graphname element item itemname field [display_number
	xoffset yoffset clock]
*/
macro xsetgraph
	if ( $# < 6 )
		echo usage : xsetgraph graphname element item itemname field
		echo [display_number xoffset yoffset clock overlay]
		EndScript
	endif
	add_output $1 ( $3 ) @( $2 ):( $4 ) $5 
	set output name $1
	set output type graph_type
	set output output_func GraphOutput
	if ( $#  > 6 )
		set output .last_plot $6
	endif
	if ( $#  > 7 )
		set output .xoffset $7
	endif
	if ( $#  > 8 )
		set output .yoffset $8
	endif
	if ( $#  > 9 )
		set output clock $9
	endif
	if ( $# > 10 )
		set output overlay $10
	endif
	
endmacro

