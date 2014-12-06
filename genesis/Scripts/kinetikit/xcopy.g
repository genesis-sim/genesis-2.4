//genesis

// Simple copy:
// Just duplicate element. No biggie.
// 
//
// Complex copy: Needs a complete interface panel of its own.
// Create a list of elms to be copied
// Move them all onto a temporary element
// Assume that messages between these elms should all be preserved.
// Find out which want their messages to external elements to be
// preserved
// Duplicate the temporary element
// Apply any common naming scheme for duplicates
// Apply any common spacing scheme (default: offset them all)
// Apply any common colouring scheme
// Apply any common addition to the notes, saying that this has
// been duplicated from so and so element.
// Duplicate any desired plots.
// Duplicate the preserved messages
// Move the original elements back to where they belong.
// Get rid of temporary element.

// x, y, z are not much use, they refer to the dest location.
function do_simple_copy(src, elm)
	str src
    str elm
	str newelm

    if ({strncmp {src} "/edit/draw" 10} == 0)
        copy  {elm} {elm}/.. -autoindex
		newelm = {el ^}

		// Check if it is an enzyme. Enzymes need messages to parent.
		if ({isa enz {newelm}})
			addmsg {newelm}/.. {newelm} ENZYME n
			addmsg {newelm} {newelm}/.. REAC eA B
		end

		position {newelm} R2 R2 R2
		callfunc {getfield {newelm} editfunc} {newelm}
    	call /edit/draw/tree RESET
    end
end

function copy_load_instructions
	xtextload /copy/copyinfo \
	"        Instructions for the copying tool: " \
	"This tool is used to perform complex copying operations. Use it" \
	"if you wish to copy several parts of a simulation, preserving" \
	"messages, and applying various rules for deriving new names," \
	"colors, etc for the copies" \
	"All messages within the set of copied elements will be preserved."\
	"It is probably not needed if you just want to copy one or two" \
	"elements. You should use the simple copy routine in such cases." \
	"This just involves dragging the source element to the 'dup'" \
	"icon on the library window." \
	"" \
	"To copy:" \
	"1. Click on source elements in edit window alongside. Their name" \
	"and a few control buttons will appear in this tool." \
	"2. If you clicked on something mistakenly, it can be moved back" \
	"simply by clicking on it again." \
	"3. Specify a destination for the copies by clicking on the" \
	"   'Destination' toggle and selecting the destination group" \
	"   with a single mouse click. The name of the destination will" \
	"   appear in the dialog. It defaults the the same group as the" \
	"   last source element." \
	"4. Specify common suffixes or prefixes for copies in the dialogs."\
	"   For example, if you specify prefix 'foo_' and suffix '.bar'," \
	"   the copy of the element 'zod' would become 'foo_zod.bar'." \
	"4. Specify common coloring scheme in the Coloring dialog." \
	"   Default is 'preserve', but you can also specify a color here."\
	"5. Specify layout of copies in the Layout dialog." \
	"   Default is 'offset', but you can also request a 'grid' layout."\
	"6. Specify text to be added to the 'NOTES' for each element." \
	"   Default is to say 'Copied from _src_ on _current date_.'" \
	"7. For each element, decide if you need to preserve messages" \
	"   coming from outside the set of copied elements. This is" \
	"   controlled by a toggle beside each element on the list." \
	"   The default is NO." \
	"8. For each element, decide if you want to duplicate plots." \
	"   The default is NO." \
	"9. Hit the 'COPY NOW' button only when all these steps have" \
	"   been completed to your satisfaction." \
	"10. Well, you didn't read the instructions did you ? That was" \
	"   silly, but we anticipated it. Hit the 'UNDO COPY' button if" \
	"   you goofed. You can even do this after you have made further" \
	"   changes to the model, as long as you haven't messed with" \
	"   groupings or altered the source elements." \
	"" \
	"As long as this window is up, you will remain in copy mode." \
	"All the usual click-drag operations will be suspended and" \
	"clicks on the edit window will instead result in copy operations."
end


function make_copyform
	create xform /copy [{EX},0,{EW},{WINHT}] -title "Copying Tool"
	ce /copy
	create xbutton Help -label "Instructions for doing copy" \
		-script "copy_load_instructions"

	create xbutton do_complex_copy -label "COPY NOW"
	create xbutton undo_complex_copy -label "UNDO COPY"
	create xtext copyinfo [0,0:last, 100%, 30:NULL.bottom] -bg white
	create xbutton HIDE -script "xhide /copy"
	disable /copy
	copy_load_instructions
end
