#include "sim_ext.h"
/* This file really belongs in sim, but is here for testing.*/

int GetActionNumber(name)
	char	*name;
{
	Action		*action;

	action=GetAction(name);
	if (action != NULL) {
		return(action->type);
	} else {
		printf("Error : Action %s not found\n",name);
		return(0);
	}
}

SoftAction(elm,action,context)
/* Context specifies whether this was called before or after the
** main action loop. If before, and the soft-action setup is preemptive,
** then this returns 1 and the original action is aborted.
*/
	Element	*elm;
	Action		*action;
	int		context;
{
	SoftActionList	*slist = elm->slist;
	int	replace=0;
	char	*Pathname();
	int doit;
	char	*targs[20];
	int	i;
/* Note that even one replace soft-action will cause the original
** to be replaced. */

	for(;slist != NULL; slist=slist->next) {	
	/* check that we want to run a func on this action before the
	** original */
		if (action->type == slist->act) {
			doit = 1;
			if (context == BEFORE_ACTIONS) {
				if (slist->context == 'p') {
				} else if (slist->context == 'r') {
					replace=1;
				} else
					 doit = 0;
			} else {
				if (slist->context != 's')
					doit = 0;
			}
			if (doit) {
			/* set up the default set of args */
				targs[0] = CopyString(slist->func);
				targs[1] = Pathname(elm);
				targs[2] = CopyString(slist->ret);
			/* set up additional args passed in with the
			** action arglist */
				for(i=0;i<action->argc ;	i++)
					targs[i+3]=action->argv[i];
				ExecuteFunction(i+3,targs);
			}
		}
	}
	return(replace);
}

add_softact(argc,argv)
	int	argc;
	char 	**argv;
{
	Element	*elm,*GetElement();
	long	act;
	char	*func;
	char	*ret = "";
	char	op = 's';
	SoftActionList	*slist,*slist2;
	
	if (argc < 3) {
		printf("usage : %s element action function [return-value [context]]\n",
			argv[0]);
		printf("context can be s[uffix], p[refix], or r[eplace]\n");
		return;
	}

	elm=GetElement(argv[1]);
	act=GetActionNumber(argv[2]);
	func=argv[3];
	if (argc >=5) ret = argv[4];
	if (argc >=6) op=argv[5][0];

	slist2 = (SoftActionList *)calloc(1,sizeof(SoftActionList));
	slist2->func=CopyString(func);
	slist2->act = act;
	slist2->ret=CopyString(ret);
	slist2->context=op;

	elm->soft_action_mask | = act;
	switch(op) {
		case 's':  /* suffix */
			if (elm->slist == NULL) {
				elm->slist = slist2;
			} else {
				for (slist = elm->slist; slist->next != NULL;
					slist=slist->next);
					slist->next = slist2;
			}
			
		break;
		case 'p':	/* prefix */
			slist2->next = elm->slist;
			elm->slist=slist2;
		break;
		case 'r':	/* replace */
			if (elm->slist->act == slist2->act) {
				slist=elm->slist;
				/* splice it out*/
				elm->slist = slist->next;
				free(slist);
			}
			slist2->next=elm->slist;
			elm->slist=slist2;
			for (slist = elm->slist; slist->next != NULL;
				slist=slist->next) {
				slist2=slist->next;
				if (slist2->act == act) {
					/* splice it out */
					slist->next = slist->next->next;
					free(slist2);
				}
			}
		break;
	}
}





delete_softact(argc,argv)
	int	argc;
	char 	**argv;
{
	Element	*elm,*GetElement();
	long	act;
	char	*func;
	char	*ret = "";
	SoftActionList	*slist,**slist2;

	if (argc < 3) {
		printf("usage : %s element action [func [retval]]\n",
			argv[0]);
		printf("action can be either the action name or 'a[ll]' to delete all soft actions\n");
		return;
	}

	elm=GetElement(argv[1]);
	if (argv[2][0] == 'a') {
		for (slist2=&(elm->slist);
		*slist2 != NULL;slist2=&((*slist2)->next)) {
			/* splice it out */
			slist = *slist2;
			*slist2 = (*slist2)->next;
			free(slist);
		}
		elm->soft_action_mask = 0;
		return;
	}

	act=GetActionNumber(argv[2]);
	if (argc >=4) func = argv[3];
	if (argc >=5) ret = argv[4];

	for (slist2=&(elm->slist);
		*slist2 != NULL;slist2=&((*slist2)->next)) {
		if ((*slist2)->act == act) {
			if (argc >= 4 && strcmp(func,(*slist2)->func) != 0)
				continue;
			if (argc >= 5 && strcmp(ret,(*slist2)->ret) != 0)
				continue;
			/* splice it out */
			slist = *slist2;
			*slist2 = (*slist2)->next;
			free(slist);
		}
	}
	/* replace the soft_action_mask to reflect the changes */
	elm->soft_action_mask = 0;
	for(slist=elm->slist;slist != NULL; slist=slist->next)
		elm->soft_action_mask |= slist->act;
}

show_softact(argc,argv)
	int	argc;
	char 	**argv;
{
	Element	*elm,*GetElement();
	long	act = 0;
	char	*func;
	char	*ret = "";
	SoftActionList	*slist,*slist2;

	if (argc < 2) {
		printf("usage : %s element [action [func]]\n",
			argv[0]);
		return;
	}

	elm=GetElement(argv[1]);
	if (argc >=3)
		act=GetActionNumber(argv[2]);
	if (argc >=4) func = argv[3];

	printf("Soft Actions for element '%s' : \n",argv[1]);
	for (slist = elm->slist; slist != NULL;
		slist=slist->next) {
		if (argc >= 3 && slist->act != act) {
			if (argc>=4 && strcmp(func,slist->func) != 0)
				continue;
		}
		printf("%s(%s,%s)\n",slist->func,
			GetActionName(slist->act),slist->ret);
	}
}

char *get_softact(argc,argv)
	int	argc;
	char 	**argv;
{
	Element	*elm,*GetElement();
	long	act;
	char	*func;
	char	*ret = "";
	int		index = 0;
	int		icount = 0;
	SoftActionList	*slist,*slist2;
	char	*softact_retfield();
	char	*id, *identifier;
	char	*field;

	if (argc < 4) {
		printf("usage : %s element identifier id [index] field\n",
			argv[0]);
		printf("identifier and field can be : a[ction], f[unction] or r[eturn_value]\n");
		printf("id is the value of the identifier. The selected\n");
		printf("field from the  soft-action defined by the identifier and index is returned\n");
	}


	elm=GetElement(argv[1]);
	identifier=argv[2];
	id=argv[3];
	if (argc==6)
		index = atoi(argv[4]);
	field = argv[argc-1];

	switch(identifier[0]) {
		case 'a' :
			act=GetActionNumber(id);
			for (slist = elm->slist; slist->next != NULL;
				slist=slist->next) {
				if (act == slist->act) {
					if (index == icount)
						return(softact_retfield(slist,field));
					icount++;
				}
			}
		break;

		case 'f' :
			for (slist = elm->slist; slist->next != NULL;
				slist=slist->next) {
				if (strcmp(id,slist->func) == 0) {
					if (index == icount)
						return(softact_retfield(slist,field));
					icount++;
				}
			}
		break;

		case 'r' :
			for (slist = elm->slist; slist->next != NULL;
				slist=slist->next) {
				if (strcmp(id,slist->ret) == 0) {
					if (index == icount)
						return(softact_retfield(slist,field));
					icount++;
				}
			}
		break;
	}
}

char *softact_retfield(slist,field)
	SoftActionList	*slist;
	char	*field;
{
	switch(field[0]) {
		case 'a' :
			return(CopyString(GetActionName(slist->act)));
		case 'f' :
			return(CopyString(slist->func));
		case 'r' :
			return(CopyString(slist->ret));
	}
	return(NULL);
}





