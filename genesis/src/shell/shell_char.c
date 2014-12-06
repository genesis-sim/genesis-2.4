static char rcsid[] = "$Id: shell_char.c,v 1.2 2005/06/26 08:25:37 svitak Exp $";

/*
** $Log: shell_char.c,v $
** Revision 1.2  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:04:22  dhb
** Initial revision
**
*/


int is_eol_char(c)
char	c;
{
	return(c=='\n' || c==';'); 
}

int is_index_delimiter(c)
char	c;
{
	return(c== '[');
}

int is_space_char(c)
char	c;
{
	return(c== '\n' || c == ' ' || c=='\t' || c=='\0');
}

int is_data_char(c)
char	c;
{
	return((c >= '0' && c <= '9') || c=='+' || c=='-' || c=='.') ;
}

int is_num(c)
char	c;
{
	return((c >= '0' && c <= '9') || c=='+' || c=='-' ) ;
}

int is_var_char(c)
char	c;
{ 
	return((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c=='_')
	|| c==',');
}

int is_str_delimiter(c)
char	c;
{
	return(c=='"');
}

int is_str_var_delimiter(c)
char	c;
{
	return(c=='*');
}

int is_alphanum(c)
char	c;
{
    return((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c=='_') ||
    (c >= '0' && c <= '9'));
}

int is_alpha(c)
char	c;
{
    return((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c=='_'));
}

int IsWhiteSpace(c)
char c;
{ return((c == ' ') || (c == '\t') || (c == '\n')); }

int IsStringDelimiter(c)
char c;
{ return((c == '\'') || (c == '"')); }

int IsStringEnd(c)
char c;
{ return((c == '\0')); } 

