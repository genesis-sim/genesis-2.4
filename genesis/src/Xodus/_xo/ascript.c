#include sim_ext.h
/* This function really belongs in sim, but is here for testing.*/

add_script(argc,argv)
	int	argc;
	char 	**argv;
{
	Element	*elm,*GetElement();
	long	act,GetActionNumber();
	char	*script;
	char	*ret = "";
	char	op = 's';
	ScriptList	*slist,*slist2;

	elm=GetElement(argv[1]);
	act=GetActionNumber(argv[2]);
	script=argv[3];
	if (argc >=5) ret = argv[4];
	if (argc >=6) op=argv[5][0];

	slist2 = (ScriptAction *)calloc(1,sizeof(ScriptAction));
	slist2->script=script;
	slist2->act = act;
	slist2->ret=ret;

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
			if ((slist = elm->slist->act) == slist2->act) {
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





drop_script(argc,argv)
	int	argc;
	char 	**argv;
{
	Element	*elm,*GetElement();
	long	act,GetActionNumber();
	char	*script;
	char	*ret = "";
	ScriptList	*slist,*slist2;

	if (argc < 3) {
		printf("usage : %s element action [script [retval]]
	}

	elm=GetElement(argv[1]);
	act=GetActionNumber(argv[2]);
	if (argc >=4) script = argv[3];
	if (argc >=5) ret = argv[4];

	slist=elm->slist;
	if (slist == NULL) return;
	for (slist2 = slist; slist2 != NULL;slist2=slist2->next) {
		if (slist2->act == act) {
			if (argc >= 4 && strcmp(script,slist2->script) != 0)
				continue;
			if (argc >= 5 && strcmp(ret,slist2->ret) != 0)
				continue;
			/* splice it out */
			slist2->next = slist2->next->next;
			free(slist2);
		}
	}
}




