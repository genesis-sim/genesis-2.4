/* These structures are used to hold unsatisfied remote requests */

typedef struct _RcmdDone {
  int src_node;			/* the node making the request */
  int src_tid;			/* it's PVM tid */
  int cnt;			/* the number of commands to be done */
} RcmdDone;

typedef struct list_obj {
  int type;
  void * object;
  struct list_obj * next;
} ListObj;
