//genesis

addescape [A 		<^P>				-id "up arrow"
addescape [B 		<^N>				-id "down arrow"
addescape [C 		<^F>				-id "right arrow"
addescape [D 		<^H>				-id "left arrow"
addescape [1~ 		"execute movebol"	-exec	-id Find
addescape [2~ 		<^I>				-id "Insert Here"
addescape [3~ 		<^D>				-id Remove
addescape [11~ 		stop			-exec	-id F1
addescape [17~ 		"status -process"	-exec	-id F6
addescape [18~ 		status			-exec	-id F7
addescape [28~ 		"commands | more"	-exec	-id Help
addescape [29~ 		step<CR>			-id Do
