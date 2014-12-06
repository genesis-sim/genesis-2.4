#ifndef _Tree_leaf_h
#define _Tree_leaf_h

/************************************************************
  
  TreeLeaf struct.

*/
/* This struct is used to pass info between the xtree and the Tree */
struct xtree_leaf_type {
	float	xpts,ypts,zpts;
	char	*ptname;
	int		shapeindex;
	int		shapeflags;
	long	fg;
	long	textfg;
	char	*elm;
};

#endif
