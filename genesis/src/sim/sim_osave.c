static char rcsid[] = "$Id: sim_osave.c,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $";

/*
** $Log: sim_osave.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  1993/07/21 21:32:47  dhb
** fixed rcsid variable type
**
 * Revision 1.1  1992/10/27  20:23:08  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"
#include "hash.h"

#ifdef LATER

#define HEADER_SIZE	100

static FILE	*name_fp;
static FILE	*user_fp;
static char	save_header[HEADER_SIZE];

/*
****************************************************************************
** basic low level routines for writing out data blocks with the necessary
** header information on each block
****************************************************************************
*/
SaveData(data,size,fp)
char	*data;
int	size;
FILE	*fp;
{
int	ID;

    /*
    ** write the data header
    */
    ID = (int)(&data);
    fwrite(&ID,sizeof(int),1,fp);
    fwrite(&size,sizeof(int),1,fp);
    /*
    ** write the data block
    */
    fwrite(data,size,1,fp);
}

WriteHeader()
{
    fwrite(save_header,HEADER_SIZE*sizeof(char),1,save_fp);
}

FILE *OpenFile(filename)
char	*filename;
{
FILE	*fp;
    if((fp = fopen(filename,"w")) == NULL){
	printf("could not open '%s' for writing\n",filename);
	return(NULL);
    };
    return(fp);
}

CloseSave()
{
    fclose(save_fp);
}

/*
****************************************
** specific simulation saving routines
****************************************
*/

SaveString(name,fp)
char 	*name;
FILE	*fp;
{
    /*
    ** search the name list for the address
    ** If found then dont bother saving it.
    */
    if(FindName(name)){
	return;
    }
    /*
    ** otherwise add it to the list and save it
    */
    AddName(name);
    SaveData(name,strlen(name)+1,fp);
}

SaveSlot(slot,fp)
Slot	*slot;
FILE	*fp;
{
    SaveData(slot,sizeof(Slot),fp);
    SaveString(slot->name,name_fp);
}

SaveMsg(msg,fp)
MsgIn	*msg;
FILE	*fp;
{
    SaveData(msg,sizeof(MsgIn),fp);
    for(i=0;i<nslots;i++){
	SaveSlot(msg->slot+i);
    }
}

SaveElement(element,fp)
Element	*element;
FILE	*fp;
{
MsgIn	*msg;
int	size;

    size = Size(element);
    SaveData(element,size,fp);
    SaveString(element->name,name_fp);
    for(msg=element->msg_in;msg;msg=msg->next){
	SaveMsg(msg,fp);
    }
}

SaveTree(element,fp)
Element	*element;
FILE	*fp;
{
ElementList	*list;
int		i;

    list = (ElementList *)calloc(1,sizeof(ElementList));
    TreeToElementList(element,list,0,0);
    for(i=0;i<list->nelements;i++){
	SaveElement(list->element[i],fp);
    }
    FreeElementList(list);
}

SaveArgList(argc,argv,fp)
int	argc;
char	**argv;
FILE	*fp;
{
int	i;

    for(i=0;i<argc;i++){
	/* save the argument strings */
	SaveData(argv[i],strlen(argv[i])+1,fp);
    }
}

SaveActionList(list,fp)
ActionList	*list;
FILE	*fp;
{
char	*name;

    SaveData(list,sizeof(ActionList),fp);
    for(;list;list=list->next){
	SaveString(list->name,name_fp);
	/*
	** save the function name just in case the function address
	** becomes invalid due to a recompile 
	*/
	name = GetFuncName(list->function);
	SaveData(name,strlen(name) + 1,name_fp);
    }
}

SaveMsgList(list,fp)
MsgList	*list;
FILE	*fp;
{
char	*name;

    SaveData(list,sizeof(MsgList),fp);
    for(;list;list=list->next){
	SaveString(list->name,name_fp);
	SaveArgList(list->slots,list->slotname,fp);
    }
}

SaveFieldList(list,fp)
FieldList	*list;
FILE	*fp;
{
char	*name;

    SaveData(list,sizeof(FieldList),fp);
    for(;list;list=list->next){
	SaveString(list->name,name_fp);
    }
}

SaveObject(object,fp)
GenesisObject	*object;
FILE	*fp;
{
char	*name;

    /* save the object */
    SaveData(object,sizeof(GenesisObject),fp);

    /* save the name and type strings */
    SaveData(object->name,strlen(object->name) + 1,fp);
    SaveData(object->type,strlen(object->type) + 1,fp);

    /*
    ** save the function name just in case the function address
    ** becomes invalid due to a recompile 
    */
    name = GetFuncName(object->function);
    SaveData(name,strlen(name) + 1,fp);

    /* save the environment list */
    SaveArgList(object->enc,object->env,fp);

    /* save the action list */
    SaveActionList(object->valid_actions,fp);

    /* save the msg list */
    SaveMsgList(object->valid_msgs,fp);

    /* save the field list */
    SaveFieldList(object->public_fields,fp);

    /* save the description */
    SaveData(object->description,strlen(object->description)+1,fp);
}

SaveObjects(fp)
FILE	*fp;
{
int	i;
extern HASH *object_hash_table;
GenesisObject	*object;
int	count = 0;

    for(i=0;i<object_hash_table->size;i++){
	if(object_hash_table->entry[i].key){
	    count++;
	}
    }
    SaveData(&count,sizeof(int),fp);
    /*
    ** go through the object hash table and save all of the 
    ** objects in it
    */
    for(i=0;i<object_hash_table->size;i++){
	if(object = (GenesisObject *)(object_hash_table->entry[i].data)){
	    SaveObject(object,fp);
	}
    }
}

SaveClasses(fp)
FILE	*fp;
{
int	i;
extern HASH *class_hash_table;
int	class;
char	*name;
int	count = 0;

    /*
    ** go through the object hash table and save all of the 
    ** objects in it
    */
    for(i=0;i<class_hash_table->size;i++){
	if(class_hash_table->entry[i].key){
	    count++;
	}
    }
    SaveData(&count,sizeof(int),fp);
    for(i=0;i<class_hash_table->size;i++){
	if(name = class_hash_table->entry[i].key){
	    class = *((int *)(object_hash_table->entry[i].data));

	    /* save the name and class */
	    SaveData(name,strlen(name)+1,fp);
	    SaveData(&class,sizeof(int),fp);
	}
    }
}

SaveSimulation(filename)
char	*filename;
{
char	name_file[100];
char	user_file[100];
char	object_file[100];
char	element_file[100];
char	header_file[100];

FILE	*fp;
    
    /*
    ** open the running data files
    */
    sprintf(name_file,"%s.name",filename);
    name_fp = OpenFile(ext);

    sprintf(user_file,"%s.user",filename);
    user_fp = OpenFile(ext);

    /*
    ** save the objects
    */
    printf("saving objects...\n");
    sprintf(object_file,"%s.obj",filename);
    fp = OpenFile(object_file);
    SaveClasses(fp);
    SaveObjects(fp);
    fclose(fp);

    /*
    ** save the elements
    */
    printf("saving elements...\n");
    sprintf(element_file,"%s.elm",filename);
    fp = OpenFile(element_file);
    SaveTree(RootElement(),fp);
    fclose(fp);


    /*
    ** write the header
    */
    sprintf(header_file,"%s.head",filename);
    fp = OpenFile(header_file);
    WriteHeader(fp);
    fclose(fp);

    /*
    ** write the complete file
    */
    sprintf(string,"cat %s %s %s %s > %s",
	header_file,
	name_file,
	object_file,
	element_file,
	filename);
    system(string);

}

#endif
