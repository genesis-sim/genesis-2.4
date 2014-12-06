static char rcsid[] = "$Id: out_add.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: out_add.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  1997/07/18 03:06:15  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/26 18:03:19  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:03:18  dhb
** Initial revision
**
*/

#include "out_ext.h"

char *TypeIntToStr(type)
int type;
{
    if(type == CHAR){
	return("char");
    } else
    if(type == SHORT){
	return("short");
    } else
    if(type == INT){
	return("int");
    } else
    if(type == FLOAT){
	return("float");
    } else
    if(type == DOUBLE){
	return("double");
    } else
    return("");
}

int TypeStrToInt(type)
char *type;
{
    if(type == NULL){
	return(INVALID);
    }
    if(strcmp(type,"char") == 0){
	return(CHAR);
    } else
    if(strcmp(type,"short") == 0){
	return(SHORT);
    } else
    if(strcmp(type,"int") == 0){
	return(INT);
    } else
    if(strcmp(type,"float") == 0){
	return(FLOAT);
    } else
    if(strcmp(type,"double") == 0){
	return(DOUBLE);
    }
    return(INVALID);
}

int CountOutputData(output_data)
struct output_data_type *output_data;
{
int i = 0;

    for(;output_data;output_data = output_data->next) i++;
    return(i);
}

int AddOutputData(output,output_data)
Output *output;
struct output_data_type *output_data;
{
struct output_data_type *data;

    if(output == NULL || output_data == NULL)
	return(0);
    /*
    ** find the end of the output data list
    */
    for(data=output->data;(data && data->next);data=data->next);
    /*
    ** link the new connection into the src list
    */
    if(data == NULL){
	output->data = output_data;
    } else {
	data->next = output_data;
    }
    return(1);
}

struct output_data_type *AssignOutputData(output,element,dataptr,datatype,datasize)
Output		*output;
Element 	*element;
int 		*dataptr;
int		datatype;
int		datasize;
{
struct output_data_type *output_data;

    if(output != NULL){
	output_data = (struct output_data_type *)
	calloc(1,sizeof(struct output_data_type));
	output_data->element = element;
	output_data->x = element->x;
	output_data->y = element->y;
	output_data->z = element->z;
	output_data->datatype = datatype;
	output_data->datasize = datasize;
	output_data->dataptr = (float *)dataptr;
	AddOutputData(output,output_data);
	return(output_data);
    } else {
	return(NULL);
    }
}

/* LATER finish up AddOutput */
#ifdef LATER
AddOutput(output,pathname,field,tree,connection_index)
Output 		*output;
char		*pathname;
char 		*field;
int 		tree;
int		connection_index;
{
Element		*element;
int 		offset;
char 		*adr;
Info 		info;
short 		stk;
Connection 	*connection;
Projection 	*projection;
int		datasize;
int		datatype;

    if(output == NULL || pathname == NULL || field == NULL){
	return(0);
    }
    /*
    ** backward compatibility with old field notation
    */
    if(field[0] == '.') field++;
    /*
    ** add the data to the output
    */
    if(HasWildcard(pathname)){
	/*
	** apply it to a tree
	*/
	stk = PutElementStack(&root_element);
	while((element = NextElement(1,0,stk)) != NULL){
	    if(MatchWildcard(Pathname(element),pathname)) {
		offset = CalculateOffset(Type(element),field,&info);
		adr = (char *)element;
	    }
	    if(adr == NULL){
		FreeElementStack(stk);
		return(0);
	    }
	    datasize = info.type_size;
	    datatype = GetDatatype(info.type);
	    /*
	    ** if the output was found then check its characteristics
	    ** against the new data
	    */
	    if(output->datatype != 0){
		if(output->datasize != datasize || 
		output->datatype != datatype){
		    Warning();
		    if(debug > 0){
			printf("mixed data types in the output\n");
		    }
		    output->mixed_data = 1;
		}
	    } else {
		output->datasize = datasize;
		output->datatype = datatype;
	    }
	    if(datatype == INVALID){
		Error();
		printf("unknown datatype %s\n",info.type);
		return(0);
	    }
	    AssignOutputData(output,element,adr + offset,datatype,datasize);
	}
	FreeElementStack(stk);
    } else {
	element = GetElement(pathname);
	offset = CalculateOffset(element->object->type,field,&info);
	adr = (char *)element;
	if(adr == NULL){
	    return(0);
	}
	if(offset == -1){
	    return(0);
	}
	datasize = info.type_size;
	datatype = GetDatatype(info.type);
	/*
	** if the output was found then check its characteristics
	** against the new data
	*/
	if(output->datatype != 0){
	    if(output->datasize != datasize || 
	    output->datatype != datatype){
		printf("data type mismatch with existing output data\n");
		Warning();
		if(debug > 0){
		    printf("mixed data types in the output\n");
		}
		output->mixed_data = 1;
	    }
	} else {
	    output->datasize = datasize;
	    output->datatype = datatype;
	}
	if(datatype == INVALID){
	    Error();
	    printf("unknown datatype %s\n",info.type);
	    return(0);
	}
	AssignOutputData(output,element,adr + offset,datatype,datasize);
    }
    return(1);
}

void do_add_output(argc,argv)
int 	argc;
char 	**argv;
{
Output		*output;
char 		*field;
char 		*output_name;
char 		*pathname;
int 		tree = 0;
int		connection = -1;
int 		nxtarg;
int		status;

    initopt(argc, argv, "output-element path field -connection # -tree");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	/*
	** get the optional arguments
	*/
	if(strcmp(G_optopt,"-connection") == 0){
	    connection = atoi(optargv[1]);
	} else if(strcmp(G_optopt,"-tree") == 0){
	    /*
	    ** this is a tree operation
	    */
	    tree = 1;
	}
      }

    if(status < 0){
	printoptusage(argc, argv);
	return;
    }

    output_name = optargv[1];
    /*
    ** try and find the output structure
    */
    if((output = (Output *)GetElement(output_name)) == NULL){
	Error();
	printf("%s: cant find output '%s'\n",optargv[0],output_name);
	return;
    }
    /*
    ** element or tree to be used
    */
    pathname = optargv[2];
    /*
    ** field to be used
    */
    field = optargv[3];
    /*
    ** add the data to the output
    */
    if(!AddOutput(output,pathname,field,tree,connection)){
	printf("unable to add output %s\n",output_name);
    }
}

#endif
