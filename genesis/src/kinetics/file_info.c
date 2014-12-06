static char rcsid[] = "$Id: file_info.c,v 1.6 2005/10/14 17:13:08 svitak Exp $";
 
/*
 *$Log: file_info.c,v $
 *Revision 1.6  2005/10/14 17:13:08  svitak
 *Included sys/types.h needed by dirent.h on some systems (OSX).
 *
 *Revision 1.5  2005/10/04 22:05:13  svitak
 *Merged branch tagged rel-2-3beta-fixes back to main trunk. All comments from
 *the branch modifications should be included.
 *
 *Revision 1.4.2.1  2005/08/23 18:53:39  svitak
 *Added include of unistd.h for close() decl.
 *
 *Revision 1.4  2005/07/29 16:13:15  svitak
 *Various changes for MIPSpro compiler warnings.
 *
 *Revision 1.3  2005/06/20 21:08:31  svitak
 *Changed sprng() calls to G_RNG(). This is so the kinetics code can still
 *be compiled even if sprng cannot.
 *
 *Revision 1.2  2005/06/16 23:20:58  svitak
 *Feb 2004 fixes by Clayton Fan and Upi Bhalla for compiling with gcc
 *under Solaris 8.
 *
 *Revision 1.1.1.1  2005/06/14 04:38:34  svitak
 *Import from snapshot of CalTech CVS tree of June 8, 2005
 */

/*
** Old log.
** Revision 1.2  2003/05/30 22:25:25  gen-dbeeman
** New version of kinetics library from Upi Bhalla, April 2003
**
*
*/

#include <sys/types.h>	/* needed by dirent.h on some systems. */
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>	/* close() */
#include "NULLArgv.h"
#include "sim_ext.h"

/*
 * Alphabetic order comparison routine for those who want it.
 */
static int
alphsort(d1, d2)
	struct dirent **d1;
	struct dirent **d2;
{
	return(strcmp((*d1)->d_name, (*d2)->d_name));
}

/* This file is for obtaining file information for GENESIS
** e.g., for directory listings and to check on status of a given file.
*/

int do_file_exists(int argc, char** argv) {
	int n;

	initopt(argc, argv, "filename");
	if (G_getopt(argc, argv) < 0) {
		printoptusage(argc,argv);
		return 0;
	}
	n = open(optargv[1], O_RDONLY);
	if (n < 0)
		return 0;
	close(n);
	return 1;
}

char** do_dir_list(int argc, char** argv) {
	int i;
	int n;
	struct dirent** namelist;
	char **ret;

#ifdef Solaris
        ret = (char**)(NULLArgv());   /* proper cast added by Upi Bhalla */
	printf("Warning: do_dir_list not supported on Solaris\n");
#else
	initopt(argc, argv, "directory_path");
	if (G_getopt(argc, argv) < 0) {
		printoptusage(argc,argv);
		return (char**)(NULLArgv());
	}
	n = scandir(optargv[1], &namelist, 0, alphsort);
	if (n <= 0)
		return (char**)NULLArgv();
	ret = (char **)malloc((1 + n) * sizeof(char *));
	for (i = 0; i < n; i++) {
		ret[i] = (char *)malloc((1 + strlen(namelist[i]->d_name)) * sizeof(char));
		strcpy(ret[i], namelist[i]->d_name);
	}
	ret[n] = (char *)NULL;
#endif
	return ret;
}

int do_max_file_version(int argc, char** argv) {
	int i;
	int n;
	struct dirent** namelist;
	int len;
	char *f;
	int version = 0;
        char **ret;        /* added by C.Fan */
#ifdef Solaris
        ret = (char**)(NULLArgv());  /* proper cast added by Upi Bhalla */
	printf("Warning: do_max_file_version not supported on Solaris\n");
#else
	initopt(argc, argv, "directory_path file_basename");
	if (G_getopt(argc, argv) < 0) {
		printoptusage(argc,argv);
		return 0;
	}
	n = scandir(optargv[1], &namelist, 0, alphsort);
	f = optargv[2];
	len = strlen(f);

	for (i = 0; i < n; i++) {
		if (strncmp(f, namelist[i]->d_name, len) == 0) {
			int j = atoi(namelist[i]->d_name + len);
			if (version < j)
				version = j;
		}
	}
#endif
	return version;
}
