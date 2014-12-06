//genesis
//
// $ProjectVersion: Release2-2.17 $
// 
// $Id: mapping.g 1.13 Tue, 21 Feb 2006 16:51:16 +0100 hugo $
//

//////////////////////////////////////////////////////////////////////////////
//'
//' Purkinje tutorial
//'
//' (C) 1998-2002 BBF-UIA
//' (C) 2005-2006 UTHSCSA
//'
//' functional ideas ... Erik De Schutter, erik@tnb.ua.ac.be
//' genesis coding ..... Hugo Cornelis, hugo.cornelis@gmail.com
//'
//' general feedback ... Reinoud Maex, Erik De Schutter, Dave Beeman, Volker Steuber, Dieter Jaeger, James Bower
//'
//////////////////////////////////////////////////////////////////////////////


// mapping.g : mapping between dendrites and spines

int include_mapping

if ( {include_mapping} == 0 )

	include_mapping = 1


///
/// SH:	MappingCreate
///
/// PA:	path..:	path with spines
///
/// DE:	Create mapping elements between dendrites and spines
///	Mappings are created within the element /mappings
///

function MappingCreate(path)

str path

	//- go to specified path

	pushe {path}

	//- create a neutral mapping element

	create neutral /mappings

	//- add a field for number of mappings

	addfield /mappings mappingCount \
		-description "Number of mappings"

	//- loop over all compartments in the path

	str comp

	foreach comp ( {el ./#[][TYPE=compartment]} )

		//- index of the element defaults to zero

		int index = 0

		//- default : do not create a mapping for this element

		int bCreateMapping = 0

		//- get the tail of the element

		str tail = {getpath {comp} -tail}

		//- calculate the place of the index count

		// the quotes are not necessary but this way emacs is somewhat
		// more friendly when matching braces

		int indexOpen = {findchar {tail} "["}
		int indexClose = {findchar {tail} "]"}

		//- if this element has an index count

		if ({indexOpen} != -1)

			//- remove the index from the element

			str compNoIndex = {substring {tail} 0 {indexOpen - 1}}

			//- if it is not a spine

			if ({strcmp {compNoIndex} spine} != 0)

				//- calculate the index

				index = {substring {tail} \
						{indexOpen + 1} \
						{indexClose - 1}}

				//- remember to create a mapping

				bCreateMapping = 1
			end
		end

		//- if we should create a mapping

		if (bCreateMapping)

			//- if the mapping already exists

			if (! {exists /mappings/{compNoIndex}})

				//- create a neutral mapping element

				create neutral /mappings/{compNoIndex}

				//- add field for indices

				addfield /mappings/{compNoIndex} indexCount \
					-description "Number of elements - 1"

				//- add field for number of synapses

				addfield /mappings/{compNoIndex} synapseCount \
					-description "Number of synapses"

				//- add field for synapse begin

				addfield /mappings/{compNoIndex} synapseBegin \
					-description "Synapse begin index"

				//- add field for synapse end

				addfield /mappings/{compNoIndex} synapseEnd \
					-description "Synapse end index"

				//- clear the field

				setfield /mappings/{compNoIndex} \
					synapseCount 0
			end

			//- set index field

			setfield /mappings/{compNoIndex} \
				indexCount {index}
		end
	end

	//- loop over all mappings with less than 30 indices

	foreach comp ( {el /mappings/#[][indexCount<30]} )

		//- delete the mapping element

		delete {comp}
	end

	//- loop over all created mapping elements

	str mapping

	foreach mapping ( {el /mappings/#[]} )

		//- get tail of mapping

		str mappingTail = {getpath {mapping} -tail}

		//echo Source elements : {path}/{mappingTail}

		//- initialize synapse begin and end index
		// because the Genesis reference manual does not document the
		// ranges for the different numerical types (int, float),
		// I use here 32767 as it is certainly big enough

		int synapseBegin = 32767
		int synapseEnd = -1

		//- loop over the source array for this mapping

		str source

		foreach source ( {el {path}/{mappingTail}[]} )

			//- get the spine that gives messages to the element

			str spine = {getmsg {source} -outgoing -destination 7}

			//- get tail of spine

			str spineTail = {getpath {spine} -tail}

			//echo spine tail : {spineTail}

			//- if we are handling a spine

			if ( {strncmp {spineTail} "spine" 5} == 0 )

				//- create a neutral compartment for the spine

				create neutral \
					/mappings/{mappingTail}/{spineTail}

				//echo {source} -> {spineTail}

				//- if this is the first spine

				if (spineTail == "spine")

					//- add index [0]

					spineTail = "spine[0]"

				end

				//- get index of synapse

				int synapseIndex \
					= {substring \
						{spineTail} \
						6 \
						{{strlen {spineTail}} - 1}}

				//- if the synapse index is the lowest so far

				if (synapseIndex < synapseBegin)

					//- remember the synapse begin

					synapseBegin = {synapseIndex}
				end

				//- if the synapse index is the highest so far

				if (synapseIndex > synapseEnd)

					//- remember synapse end

					synapseEnd = {synapseIndex}
				end
			end
		end

		//- set field for number of synapses and synaptic range

		setfield {mapping} \
			synapseCount {NumberOfElements {mapping}/} \
			synapseBegin {synapseBegin} \
			synapseEnd {synapseEnd}
	end

	//- go to previous current element

	pope
end


///
/// SH:	MappingDelete
///
/// PA:	path..:	path with spines
///
/// DE:	Delete mappings created by MappingCreate
///

function MappingDelete

	//- delete mappings element

	delete /mappings
end


end


