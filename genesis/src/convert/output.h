#ifndef OUTPUT_H
#define OUTPUT_H

#ifdef __STDC__
void InsertItemInArgList ( char* argv[], int loc, char* item, int max );
void AddItemToArgList    ( char* argv[], char* item, int max );
void Output              ( );	/* FIXME. Should use varargs. */
void OutputIndentation   ( void );
void SetOutputIndentation( char* newIndentation );
void OutputArgList       ( char* argv[] );
#else
extern void AddItemToArgList();
extern void InsertItemInArgList();
extern void Output();
extern void OutputArgList();
extern void OutputIndentation();
extern void SetOutputIndentation();
#endif

#endif
