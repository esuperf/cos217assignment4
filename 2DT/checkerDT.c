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

// static int DynArray_isValid(DynArray_T oDynArray)
//    {
//    if (oDynArray->uPhysLength < MIN_PHYS_LENGTH) return 0;
//    if (oDynArray->uLength > oDynArray->uPhysLength) return 0;
//    if (oDynArray->ppvArray == NULL) return 0;
//    return 1;
//    }

/* see checkerDT.h for specification */
boolean CheckerDT_Node_isValid(Node_T oNNode) {
   Node_T oNParent;
   Node_T oNChild;
   Path_T oPNPath;
   Path_T oPPPath;

   /* Sample check: a NULL pointer is not a valid node */
   if(oNNode == NULL) {
      fprintf(stderr, "A node is a NULL pointer\n");
      return FALSE;
   }

   /* Sample check: parent's path must be the longest possible
      proper prefix of the node's path */
   oNParent = Node_getParent(oNNode);
   if(oNParent != NULL) {
      oPNPath = Node_getPath(oNNode);
      oPPPath = Node_getPath(oNParent);

      if(Path_getSharedPrefixDepth(oPNPath, oPPPath) !=
         Path_getDepth(oPNPath) - 1) {
         fprintf(stderr, "P-C nodes don't have P-C paths: (%s) (%s)\n",
                 Path_getPathname(oPPPath), Path_getPathname(oPNPath));
         return FALSE;
      }

   }

   /*same for child?*/
   oNChild = Node_getChild(oNNode);
   if(oNChild != NULL) {
      oPNPath = Node_getPath(oNNode);
      oPCPath = Node_getPath(oNChild);

      if(Path_getSharedPrefixDepth(oPNPath, oPCPath) !=
         Path_getDepth(oPNPath) + 1) {
         fprintf(stderr, "N-C nodes don't have N-C paths: (%s) (%s)\n",
                 Path_getPathname(oPNPath), Path_getPathname(oPCPath));
         return FALSE;
      }
   }

   if(Path_compareString(Node_getPath(oNNode), oNNode->oPPath) == 0){
      fprintf(stderr, "oPPath is not the absolute path")
      return FALSE;
   }

   if(Path_comparePath(Node_getParent(oNNode)->oPPath, oNNode->oNParent->oPPath) != 0) {
      fprintf(stderr, "issue with oNParent");
      return false;
   }

   if(DynArray_getLength(oNNode->oDChildren) != Node_getNumChildren(oNNode)){
      fprintf(stderr, "number of children... is not the number of children");
      return false;
   }


   
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
   //Path_T checkedPath = Node_getPath(checkedNode);
   //Path_T comparedPath;
   Node_T child1;
   Node_T child2;
   Node_T *child1Ptr;
   Node_T *child2Ptr;

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
      

         // checking if there are any duplicate paths
         // Path_T comparedPath = Node_getPath(oNChild);
         // if(Path_comparePath(comparedPath, checkedPath) == 0){
         //    fprintf(stderr, "this path already exists!!!")
         //    return FALSE;
         // }

         if(iStatus != SUCCESS) {
            fprintf(stderr, "getNumChildren claims more children than getChild returns\n");
            return FALSE;
         }
         
         //child checks
         child1 = Node_getChild(oNNode, ulIndex, child1Ptr);
         child2 = Node_getChild(oNNode, ulIndex + 1, child2Ptr);
         fprintf(stderr, "testing");
         if(Path_comparePath(Node_getPath(child1Ptr),
          Node_getPath(child2Ptr)) > 0) {
            fprintf(stderr, "children must be in lexicographical order");
            return false;
         }
         if(Path_comparePath(child1Ptr->opPath, child2Ptr->opPath) == 0) {
            fprintf(stderr, "children can't have the same name!!");
            return false;
         }

         /* if recurring down one subtree results in a failed check
            farther down, passes the failure back up immediately */
         if(!CheckerDT_treeCheck(oNChild))
            return FALSE;
      }


   }
   return TRUE;
}

/* see checkerDT.h for specification */
boolean CheckerDT_isValid(boolean bIsInitialized, Node_T oNRoot,
                          size_t ulCount) {
   size_t counter = 0;
   Node_T child1;
   Node_T child2;
   Node_T *child1Ptr;
   Node_T *child2Ptr;

   /* Sample check on a top-level data structure invariant:
      if the DT is not initialized, its count should be 0. */
   if(!bIsInitialized){
      if(ulCount != 0) {
         fprintf(stderr, "Not initialized, but count is not 0\n");
         return FALSE;}
      if(oNRoot != NULL){
         fprintf(stderr, "Not inititialized, but root exists\n");
      }
      }

   /* Now checks invariants recursively at each node from the root. */
   if (!CheckerDT_treeCheck(oNRoot, &counter)){
      return FALSE;
   }

   
   /*Checks if ulCount is valid*/
   if (ulCount != *counter) {
      fprintf(stderr, "Total node count can't be less than the 
         number of nodes in the tree");
   }

   if (DT_contains(oNRoot->oPPath) == FALSE) {
      fprinf(stderr, "DT_contains is broken");
   }


   // for (i = 0; i < ulCount; i--){
   //    child1 = Node_getChild(oNNode, i, child1Ptr);
   //    child2 = Node_getChild(oNNode, i+1, child2Ptr);
   //       if(Path_comparePath(Node_getPath(child1Ptr)),
   //        Path_comparePath(child2Ptr) > 0) {
   //          fprintf(stderr, "children must be in lexicographical order");
   //          return false;
   //       }
   //       if(Path_comparePath(child1Ptr->opPath, child2Ptr->opPath) == 0) {
   //          fprintf(stderr, "children can't have the same name!!");
   //          return false;
   //       } 
   // }

   // if(DynArray_isValid(DynArray_new(size_t uLength)) == 0){
   //    fprintf(stderr, "Issue with Dynarray creation");
   //    return FALSE;
   //}
}
