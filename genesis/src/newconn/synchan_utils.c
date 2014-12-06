static char rcsid[] = "$Id: synchan_utils.c,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $";

/*
** $Log: synchan_utils.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/06/29 21:17:20  mhucka
** Loads of un-ANSIfication to support old compilers like SunOS 4's cc.
**
** Revision 1.2  2001/04/18 22:39:37  mhucka
** Miscellaneous portability fixes, mainly for SGI IRIX.
**
** Revision 1.1  2000/04/27 07:30:49  mhucka
** First version, from Mike Vanier.
**
*/

#include "newconn_ext.h"
#include <assert.h>

static char     synchan_passback[1000];  /* Long string for SHOW passbacks. */


/*
 * Synchan_SET
 *
 * FUNCTION
 *     This is an action function for any kind of synchan.  It intercepts
 *     the SET action (setfield/getfield commands) and makes sure that
 *     the user is not trying to set a field in a non-existent synapse.
 *
 * ARGUMENTS
 *     char     *object_name -- the name of the element's object
 *     Synchan  *synchan     -- pointer to the element
 *     Action   *action      -- genesis action
 *
 * RETURN VALUE
 *     int -- 1 = abort the SET action; 0 = continue with the normal SET.
 *
 * AUTHOR
 *    Mike Vanier
 *
 */

int
Synchan_SET(object_name, synchan, action)
char *object_name;
struct Synchan_type *synchan;
Action *action;
{
    int     i, index, start, stop;
    char    indexstr[100];

    if (!CheckClass(synchan, ClassID("synchannel")))
    {
        Error();
        printf("%s object %s: SET: synchan is not a synchan or derived object!\n",
               object_name, Pathname((Element *)synchan));
        return 1;
    }

    if (action->argc != 2)
        return 0;   /* do the normal set */

    if (strncmp(action->argv[0], "synapse", 7) == 0)
    {
        if (action->argv[0][7] != '[')
        {
            /* We're not trying to set fields on synapse[XX] */
            return 0; /* do the normal set. */
        }

        /* Look for terminating ']'. */
        start = stop = 8;

        for (i = start; i < ((int)strlen(action->argv[0])); i++)
        {
            if (action->argv[0][i] == ']')
            {
                stop = i;
                break;
            }
        }

        if (start == stop) /* invalid field */
        {
            Error();
            printf("%s object %s: SET: invalid field: %s\n",
                   object_name, Pathname((Element *)synchan),
                   action->argv[0]);
            return 1;
        }

        assert(stop > start);

        /* Get the index. */
        (void)BZERO(indexstr, 100);
        strncpy(indexstr, &(action->argv[0][start]),
                (size_t)(stop - start));

        /* If the index string has any nondigits, it's an error. */
        for (i = 0; i < ((int)strlen(indexstr)); i++)
        {
            if ((indexstr[i] < '0') || (indexstr[i] > '9'))
            {
                Error();
                printf("%s object %s: SET: invalid synapse index: %s\n\tsynapse index must be a positive integer!\n",
                       object_name, Pathname((Element *)synchan),
                       indexstr);
                return 1;
            }
        }

        index = atoi(indexstr);

        if ((index < 0) || (index >= synchan->nsynapses))
        {
            Error();
            printf("%s object %s: SET: synapse index %d is out of range\n\t(maximum synapse index = %d for this synchan)\n",
                   object_name, Pathname((Element *)synchan),
                   index, (synchan->nsynapses - 1));
            return 1;
        }

        /* Since the synapse is valid, do the normal SET. */
        return 0;
    }
    else
    {
        return 0; /* Do the normal SET. */
    }
}




/*
 * Synchan_SHOW
 *
 * FUNCTION
 *     This is an action function for any kind of synchan.  It intercepts the
 *     SHOW action (showfield command) and makes sure that the user is not
 *     trying to examine a field from a non-existent synapse.
 *
 * ARGUMENTS
 *     char     *object_name -- the name of the element's object
 *     Synchan  *synchan     -- pointer to the element
 *     Action   *action      -- genesis action
 *
 * RETURN VALUE
 *     int -- 1 = abort the SHOW action; 0 = continue with the normal SHOW.
 *
 * AUTHOR
 *    Mike Vanier
 *
 */

int
Synchan_SHOW(object_name, synchan, action)
char *object_name;
struct Synchan_type *synchan;
Action *action;
{
    int   i, index, start, stop;
    char  indexstr[100];
    char *pb = synchan_passback;

    /*
     * WISH-I-COULD-FIXME: this action gets executed twice in a row
     * when showing just one field, for reasons that are completely
     * obscure to me.  This can result in two error messages when only
     * one should occur; otherwise, it's harmless.
     */

    if (strncmp(action->data, "synapse", 7) == 0)
    {
        if (action->data[7] != '[')
        {
            /* We're not trying to show fields on synapse[XX] */
            return 0;   /* do the normal SHOW. */
        }

        /* Look for terminating ']'. */
        start = stop = 8;

        for (i = start; i < ((int) strlen(action->data)); i++)
        {
            if (action->data[i] == ']')
            {
                stop = i;
                break;
            }
        }

        if (start == stop) /* invalid field */
        {
            Error();
            printf("%s object %s: SHOW: invalid field %s\n",
                   object_name, Pathname((Element *)synchan), action->data);
            sprintf(pb, "error");
            action->passback = pb;
            return 1;
        }

        assert(stop > start);

        /* Get the index. */
        (void)BZERO(indexstr, 100);
        strncpy(indexstr, &(action->data[start]),
                (size_t)(stop - start));

        /* If the index string has any nondigits, it's an error. */
        for (i = 0; i < ((int)strlen(indexstr)); i++)
        {
            if ((indexstr[i] < '0') || (indexstr[i] > '9'))
            {
                Error();
                printf("%s object %s: SHOW: invalid `mod' index: %s\n\t`mod' index must be a positive integer!\n",
                       object_name, Pathname((Element *)synchan), indexstr);
                sprintf(pb, "error");
                action->passback = pb;
                return 1;
            }
        }

        index = atoi(indexstr);

        if (index >= synchan->nsynapses)
        {
            Error();
            printf("%s object %s: SHOW: `mod' index %d is out of range\n\t(maximum `mod' index = %d for this synchan)\n",
                   object_name, Pathname((Element *)synchan),
                   index, (synchan->nsynapses - 1));
            sprintf(pb, "error");
            action->passback = pb;
            return 1;
        }

        return 0; /* Do the normal SHOW. */
    }
    else
    {
        return 0; /* Do the normal SHOW. */
    }
}
