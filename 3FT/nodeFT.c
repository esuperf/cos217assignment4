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

/*
  Links new child oNChild into oNParent's children array at index
  ulIndex. Returns SUCCESS if the new child was added successfully,
  or  MEMORY_ERROR if allocation fails adding oNChild to the array.
*/
/*new: child cannot be added if oNParent is a file*/
static int Node_addChild(Node_T oNParent, Node_T oNChild,
                         size_t ulIndex) {
   assert(oNParent != NULL);
   assert(oNChild != NULL);

   if(oNParent->isFile == 0){
    return NOT_A_DIRECTORY;
   }
   if(DynArray_addAt(oNParent->oDChildren, ulIndex, oNChild))
      return SUCCESS;
   else
      return MEMORY_ERROR;
}

/*should be no change from DT*/
/*
  Compares the string representation of oNfirst with a string
  pcSecond representing a node's path.
  Returns <0, 0, or >0 if oNFirst is "less than", "equal to", or
  "greater than" pcSecond, respectively.
*/
static int Node_compareString(const Node_T oNFirst,
                                 const char *pcSecond) {
   assert(oNFirst != NULL);
   assert(pcSecond != NULL);

   return Path_compareString(oNFirst->oPPath, pcSecond);
}

/* should be no change from DT*/
/*
  Compares the string representation of oNfirst with a string
  pcSecond representing a node's path.
  Returns <0, 0, or >0 if oNFirst is "less than", "equal to", or
  "greater than" pcSecond, respectively.
*/
static int Node_compareString(const Node_T oNFirst,
                                 const char *pcSecond) {
   assert(oNFirst != NULL);
   assert(pcSecond != NULL);

   return Path_compareString(oNFirst->oPPath, pcSecond);
}