/*--------------------------------------------------------------------*/
/* checkerDT.c                                                        */
/* Author:                                                            */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "checkerDT.h"
#include "dynarray.h"
#include "path.h"

 /*see checkerDT.h for specification */
boolean CheckerDT_Node_isValid(Node_T oNNode) {
   return TRUE;
}

/*
   Performs a pre-order traversal of the tree rooted at oNNode.
   Returns FALSE if a broken invariant is found and
   returns TRUE otherwise.

   You may want to change this function's return type or
   parameter list to facilitate constructing your checks.
   If you do, you should update this function comment.
*/
static boolean CheckerDT_treeCheck(Node_T oNNode, size_t *counter){
   size_t ulIndex;
   /*Path_T checkedPath = Node_getPath(checkedNode);
   //Path_T comparedPath;*/
   /*int child1;
   int child2;*/
   Node_T *child1Ptr;
   Node_T *child2Ptr;
   /*fprintf(stderr, "tree check is done\n");*/

   if(oNNode!= NULL) {
      (*counter)++;
      /* Sample check on each node: node must be valid */
      /* If not, pass that failure back up immediately */
      if(!CheckerDT_Node_isValid(oNNode))
         return FALSE;

      /* Recur on every child of oNNode */
      for(ulIndex = 0; ulIndex < Node_getNumChildren(oNNode); ulIndex++)
      {
         Node_T oNChild = NULL;
         int iStatus = Node_getChild(oNNode, ulIndex, &oNChild);
      

         /* checking if there are any duplicate paths
         Path_T comparedPath = Node_getPath(oNChild);
         if(Path_comparePath(comparedPath, checkedPath) == 0){
             fprintf(stderr, "this path already exists!!!")
             return FALSE;
          }*/

         if(iStatus != SUCCESS) {
            fprintf(stderr, "getNumChildren claims more children than getChild returns\n");
            return FALSE;
         }
         
         /*child checks*/
         Node_getChild(oNNode, ulIndex, child1Ptr);
         Node_getChild(oNNode, ulIndex + 1, child2Ptr);

         fprintf(stderr, "testing");
         if(Path_comparePath(Node_getPath(*child1Ptr),
          Node_getPath(*child2Ptr)) > 0) {
            fprintf(stderr, "children must be in lexicographical order\n");
            return FALSE;
         }
         if(Path_comparePath(Node_getPath(*child1Ptr),
          Node_getPath(*child2Ptr)) == 0) {
            fprintf(stderr, "children can't have the same name!!\n");
            return FALSE;
         }

         /* if recurring down one subtree results in a failed check
            farther down, passes the failure back up immediately */
         if(!CheckerDT_treeCheck(oNChild, counter))
            return FALSE;
      }
   }
   return TRUE;
}

/* see checkerDT.h for specification */
boolean CheckerDT_isValid(boolean bIsInitialized, Node_T oNRoot,
                          size_t ulCount) {

   return TRUE;
   /*if (DT_contains(oNRoot->oPPath) == FALSE) {
      fprintf(stderr, "DT_contains is broken");
      return FALSE;
   }*/

   /* if(DynArray_isValid(DynArray_new(size_t uLength)) == 0){
   //    fprintf(stderr, "Issue with Dynarray creation");
   //    return FALSE;
   */
}
