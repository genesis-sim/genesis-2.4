/*
 * FILE: header.h
 *
 */

#ifndef HEADER_H
#define HEADER_H


#define arg_is(s)          (strcmp(argv[nxtarg],s) == 0)
#define farg()             (atof(argv[++nxtarg]))
#define arg_starts_with(c) (argv[nxtarg][0] == c)

#define SQR(x)      ((x)*(x))

#ifdef SYSV
#define bcopy(s1, s2, length) (int)memcpy(s2, s1, length)
#endif

typedef int (*PFI) (void);


#endif  /* HEADER_H */


