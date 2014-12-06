#ifndef FMT1_H
#define FMT1_H

#define MAX_FMT1_NAME 128

typedef int fmt1_type;

#define FMT1_INT 1
#define FMT1_CHAR 2
#define FMT1_SHORT 3
#define FMT1_LONG 4
#define FMT1_FLOAT 5
#define FMT1_DOUBLE 6

extern int FMT1_GetHeaderSize();
extern int FMT1_GetDataType();
extern int FMT1_inquire();

extern int FMT1_attinq();
extern int FMT1_attget();
extern int FMT1_attput();
extern int FMT1_attname();

extern int FMT1_varinq();
extern int FMT1_varget();
extern int FMT1_varput();
extern int FMT1_varname();

extern int FMT1_ReadHeader();

#endif
