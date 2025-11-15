/*--------------------------------------------------------------------*/
/* nodeFT.c                                                           */
/* Author: Liz Superfin                                               */
/*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "dynarray.h"
#include "nodeDT.h"
#include "checkerDT.h"

/* A node in a DT */
struct node {
   /* the object corresponding to the node's absolute path */
   Path_T oPPath;
   /* this node's parent */
   Node_T oNParent;
   /* the object containing links to this node's children */
   DynArray_T oDChildren;
   /* boolean that determines whether this is a file or directory*/
   boolean isFile;
   /* stores contents if it is a file*/
   void *contents;
};

