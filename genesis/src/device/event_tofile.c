static char rcsid[] = "$Id: event_tofile.c,v 1.2 2005/06/27 19:00:35 svitak Exp $";

/*
** $Log: event_tofile.c,v $
** Revision 1.2  2005/06/27 19:00:35  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1998/03/31 22:10:24  dhb
** Initial revision
**
*/

#include "spike_ext.h"
#include "stdlib.h"


/*
** Monitors the activation state of some object via an incoming
** message and writes simulation time to file iff activation >
** threshold.
*/

void Event_tofile(evfi,action)
register struct event_tofile_type *evfi;
Action		*action;
{
	MsgIn *msg;
	float currti;

    if(debug > 1){
	ActionHeader("VDepGate",evfi,action);
    }

    SELECT_ACTION(action){
    case PROCESS:
		if(!(evfi->open)) break;
		MSGLOOP(evfi,msg) {
			case 0: /* input */
				if(MSGVALUE(msg,0) >= evfi->threshold) {
					currti = SimulationTime();
					if(evfi->fp == NULL) {
						printf("file '%s' is not open \n",evfi->fname);
					} else {
						fprintf(evfi->fp, "%f\n", currti);
					}
				}
			break;
		}
		break;
	case OPEN:
    case RESET:
		if(evfi->open) fclose(evfi->fp);
		if((evfi->fp = fopen(evfi->fname, "w")) == NULL){
			printf("unable to open file '%s' \n",evfi->fname);
			return;
		}
		evfi->open = 1;
        break;
    case CLOSE: /* closefile */
		if(evfi->open) fclose(evfi->fp);
		evfi->open = 0;
   		break;
	case FLUSH: /* flush file */
		if(evfi->open) {
			if(evfi->fp == NULL) {
				printf("file '%s' is not open \n", evfi->fname);
			} else {
				fflush(evfi->fp);
			}
		}
		break;
    }
}
