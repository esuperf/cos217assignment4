
/*--------------------------------------------------------------------*/
/* ft.c                                                           */
/* Author: Liz Superfin                                               */
/*--------------------------------------------------------------------*/

#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "dynarray.h"
#include "path.h"
#include "nodeFT.h"
#include "checkerDT.h"
#include "ft.h"

/*no change*/
/*Traverses the DT starting at the root as far as possible towards
  absolute path oPPath. If able to traverse, returns an int SUCCESS
  status and sets *poNFurthest to the furthest node reached (which may
  be only a prefix of oPPath, or even NULL if the root is NULL).
  Otherwise, sets *poNFurthest to NULL and returns with status:
  * CONFLICTING_PATH if the root's path is not a prefix of oPPath
  * MEMORY_ERROR if memory could not be allocated to complete request*/
static int FT_traversePath(Path_T oPPath, Node_T *poNFurthest) {
   int iStatus;
   Path_T oPPrefix = NULL;
   Node_T oNCurr;
   Node_T oNChild = NULL;
   size_t ulDepth;
   size_t i;
   size_t ulChildID;

   assert(oPPath != NULL);
   assert(poNFurthest != NULL);

   /* root is NULL -> won't find anything */
   if(oNRoot == NULL) {
      *poNFurthest = NULL;
      return SUCCESS;
   }

   iStatus = Path_prefix(oPPath, 1, &oPPrefix);
   if(iStatus != SUCCESS) {
      *poNFurthest = NULL;
      return iStatus;
   }

   if(Path_comparePath(Node_getPath(oNRoot), oPPrefix)) {
      Path_free(oPPrefix);
      *poNFurthest = NULL;
      return CONFLICTING_PATH;
   }
   Path_free(oPPrefix);
   oPPrefix = NULL;

   oNCurr = oNRoot;
   ulDepth = Path_getDepth(oPPath);
   for(i = 2; i <= ulDepth; i++) {
      iStatus = Path_prefix(oPPath, i, &oPPrefix);
      if(iStatus != SUCCESS) {
         *poNFurthest = NULL;
         return iStatus;
      }
      if(Node_hasChild(oNCurr, oPPrefix, &ulChildID)) {
         /* go to that child and continue with next prefix */
         Path_free(oPPrefix);
         oPPrefix = NULL;
         iStatus = Node_getChild(oNCurr, ulChildID, &oNChild);
         if(iStatus != SUCCESS) {
            *poNFurthest = NULL;
            return iStatus;
         }
         oNCurr = oNChild;
      }
      else {
         /* oNCurr doesn't have child with path oPPrefix:
            this is as far as we can go */
         break;
      }
   }

   Path_free(oPPrefix);
   *poNFurthest = oNCurr;
   return SUCCESS;
}

/*no change*/
/*Traverses the DT to find a node with absolute path pcPath. Returns a
  int SUCCESS status and sets *poNResult to be the node, if found.
  Otherwise, sets *poNResult to NULL and returns with status:
  * INITIALIZATION_ERROR if the DT is not in an initialized state
  * BAD_PATH if pcPath does not represent a well-formatted path
  * CONFLICTING_PATH if the root's path is not a prefix of pcPath
  * NO_SUCH_PATH if no node with pcPath exists in the hierarchy
  * MEMORY_ERROR if memory could not be allocated to complete request*/
static int FT_findNode(const char *pcPath, Node_T *poNResult) {
   Path_T oPPath = NULL;
   Node_T oNFound = NULL;
   int iStatus;

   assert(pcPath != NULL);
   assert(poNResult != NULL);

   if(!bIsInitialized) {
      *poNResult = NULL;
      return INITIALIZATION_ERROR;
   }

   iStatus = Path_new(pcPath, &oPPath);
   if(iStatus != SUCCESS) {
      *poNResult = NULL;
      return iStatus;
   }

   iStatus = DT_traversePath(oPPath, &oNFound);
   if(iStatus != SUCCESS)
   {
      Path_free(oPPath);
      *poNResult = NULL;
      return iStatus;
   }

   if(oNFound == NULL) {
      Path_free(oPPath);
      *poNResult = NULL;
      return NO_SUCH_PATH;
   }

   if(Path_comparePath(Node_getPath(oNFound), oPPath) != 0) {
      Path_free(oPPath);
      *poNResult = NULL;
      return NO_SUCH_PATH;
   }

   Path_free(oPPath);
   *poNResult = oNFound;
   return SUCCESS;
}
/*--------------------------------------------------------------------*/


/*Inserts a new directory into the FT with absolute path pcPath.
   Returns SUCCESS if the new directory is inserted successfully.
   Otherwise, returns:
   * INITIALIZATION_ERROR if the FT is not in an initialized state
   * BAD_PATH if pcPath does not represent a well-formatted path
   * CONFLICTING_PATH if the root exists but is not a prefix of pcPath
   * NOT_A_DIRECTORY if a proper prefix of pcPath exists as a file
   * ALREADY_IN_TREE if pcPath is already in the FT (as dir or file)
   * MEMORY_ERROR if memory could not be allocated to complete request*/
/* same as DT insert except with NOT_A_DIRECTORY option added*/
int FT_insertDir(const char *pcPath){
   int iStatus;
   Path_T oPPath = NULL;
   Node_T oNFirstNew = NULL;
   Node_T oNCurr = NULL;
   size_t ulDepth, ulIndex;
   size_t ulNewNodes = 0;

   assert(pcPath != NULL);
   assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));

   /* validate pcPath and generate a Path_T for it */
   if(!bIsInitialized)
      return INITIALIZATION_ERROR;

   iStatus = Path_new(pcPath, &oPPath);
   if(iStatus != SUCCESS)
      return iStatus;

   /* find the closest ancestor of oPPath already in the tree */
   iStatus= DT_traversePath(oPPath, &oNCurr);
   if(iStatus != SUCCESS)
   {
      Path_free(oPPath);
      return iStatus;
   }

   /*CHANGED!! checking if the path ends as a file*/
   if(Node_isFile(oCurr) == TRUE) {
    Path_free(oPPath);
    return NOT_A_DIRECTORY;
   }

   /* no ancestor node found, so if root is not NULL,
      pcPath isn't underneath root. */
   if(oNCurr == NULL && oNRoot != NULL) {
      Path_free(oPPath);
      return CONFLICTING_PATH;
   }

   if(oNCurr)

   ulDepth = Path_getDepth(oPPath);
   if(oNCurr == NULL) /* new root! */
      ulIndex = 1;
   else {
      ulIndex = Path_getDepth(Node_getPath(oNCurr))+1;

      /* oNCurr is the node we're trying to insert */
      if(ulIndex == ulDepth+1 && !Path_comparePath(oPPath,
                                       Node_getPath(oNCurr))) {
         Path_free(oPPath);
         return ALREADY_IN_TREE;
      }
   }

   /* starting at oNCurr, build rest of the path one level at a time */
   while(ulIndex <= ulDepth) {
      Path_T oPPrefix = NULL;
      Node_T oNNewNode = NULL;

      /* generate a Path_T for this level */
      iStatus = Path_prefix(oPPath, ulIndex, &oPPrefix);
      if(iStatus != SUCCESS) {
         Path_free(oPPath);
         if(oNFirstNew != NULL)
            (void) Node_free(oNFirstNew);
         assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));
         return iStatus;
      }

      /* insert the new node for this level */
      iStatus = Node_new(oPPrefix, oNCurr, &oNNewNode);
      if(iStatus != SUCCESS) {
         Path_free(oPPath);
         Path_free(oPPrefix);
         if(oNFirstNew != NULL)
            (void) Node_free(oNFirstNew);
         assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));
         return iStatus;
      }

      /* set up for next level */
      Path_free(oPPrefix);
      oNCurr = oNNewNode;
      ulNewNodes++;
      if(oNFirstNew == NULL)
         oNFirstNew = oNCurr;
      ulIndex++;
   }

   Path_free(oPPath);
   /* update DT state variables to reflect insertion */
   if(oNRoot == NULL)
      oNRoot = oNFirstNew;
   ulCount += ulNewNodes;

   assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));
   return SUCCESS;
}


/*Returns TRUE if the FT contains a directory with absolute path
  pcPath and FALSE if not or if there is an error while checking.*/
/*diff is that it needs to check if the found node is a file and\
if so return FALSE*/
boolean FT_containsDir(const char *pcPath){
    int iStatus;
   Node_T oNFound = NULL;

   assert(pcPath != NULL);

   iStatus = FT_findNode(pcPath, &oNFound);
   /*CHANGED to check if it contains a file instead of a dir*/
   if (iStatus == FALSE && Node_isFile(oNFound) == TRUE){
    return FALSE;
   }
   return (boolean) (iStatus == SUCCESS);
}


/*Removes the FT hierarchy (subtree) at the directory with absolute
  path pcPath. Returns SUCCESS if found and removed.
  Otherwise, returns:
  * INITIALIZATION_ERROR if the FT is not in an initialized state
  * BAD_PATH if pcPath does not represent a well-formatted path
  * CONFLICTING_PATH if the root exists but is not a prefix of pcPath
  * NO_SUCH_PATH if absolute path pcPath does not exist in the FT
  * NOT_A_DIRECTORY if pcPath is in the FT as a file not a directory
  * MEMORY_ERROR if memory could not be allocated to complete request*/
/*diff = need to add NOT_A_DIRECTORY*/
int FT_rmDir(const char *pcPath){
    int iStatus;
   Node_T oNFound = NULL;

   assert(pcPath != NULL);
   assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));

   iStatus = DT_findNode(pcPath, &oNFound);

   /*CHANGED added check if this is a directory*/
   if (Node_isFile(oNFound) == TRUE)
        return NOT_A_DIRECTORY;

   if(iStatus != SUCCESS)
       return iStatus;

   ulCount -= Node_free(oNFound);
   if(ulCount == 0)
      oNRoot = NULL;

   assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));
   return SUCCESS;
}

/*
   Inserts a new file into the FT with absolute path pcPath, with
   file contents pvContents of size ulLength bytes.
   Returns SUCCESS if the new file is inserted successfully.
   Otherwise, returns:
   * INITIALIZATION_ERROR if the FT is not in an initialized state
   * BAD_PATH if pcPath does not represent a well-formatted path
   * CONFLICTING_PATH if the root exists but is not a prefix of pcPath,
                      or if the new file would be the FT root
   * NOT_A_DIRECTORY if a proper prefix of pcPath exists as a file
   * ALREADY_IN_TREE if pcPath is already in the FT (as dir or file)
   * MEMORY_ERROR if memory could not be allocated to complete request
*/
/*diff if that it needs not_a_file(?)*/
/*UNFINISHED*/
int FT_insertFile(const char *pcPath, void *pvContents, size_t ulLength){
    int iStatus;
   Path_T oPPath = NULL;
   Node_T oNFirstNew = NULL;
   Node_T oNCurr = NULL;
   size_t ulDepth, ulIndex;
   size_t ulNewNodes = 0;

   assert(pcPath != NULL);
   assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));

   /* validate pcPath and generate a Path_T for it */
   if(!bIsInitialized)
      return INITIALIZATION_ERROR;

   iStatus = Path_new(pcPath, &oPPath);
   if(iStatus != SUCCESS)
      return iStatus;

   /* find the closest ancestor of oPPath already in the tree */
   iStatus= DT_traversePath(oPPath, &oNCurr);
   if(iStatus != SUCCESS)
   {
      Path_free(oPPath);
      return iStatus;
   }

   /*CHANGED!! checking if the path ends as a file*/
   if(Node_isFile(oCurr) == TRUE) {
    Path_free(oPPath);
    return NOT_A_DIRECTORY;
   }

   /* no ancestor node found, so if root is not NULL,
      pcPath isn't underneath root. */
   if(oNCurr == NULL && oNRoot != NULL) {
      Path_free(oPPath);
      return CONFLICTING_PATH;
   }


   ulDepth = Path_getDepth(oPPath);
   if(oNCurr == NULL) /* new root! */
      ulIndex = 1;
   else {
      ulIndex = Path_getDepth(Node_getPath(oNCurr))+1;

      /* oNCurr is the node we're trying to insert */
      if(ulIndex == ulDepth+1 && !Path_comparePath(oPPath,
                                       Node_getPath(oNCurr))) {
         Path_free(oPPath);
         return ALREADY_IN_TREE;
      }
   }

   /* starting at oNCurr, build rest of the path one level at a time */
   while(ulIndex <= ulDepth) {
      Path_T oPPrefix = NULL;
      Node_T oNNewNode = NULL;

      /* generate a Path_T for this level */
      iStatus = Path_prefix(oPPath, ulIndex, &oPPrefix);
      if(iStatus != SUCCESS) {
         Path_free(oPPath);
         if(oNFirstNew != NULL)
            (void) Node_free(oNFirstNew);
         assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));
         return iStatus;
      }

      /* insert the new node for this level */
      iStatus = Node_new(oPPrefix, oNCurr, &oNNewNode);
      if(iStatus != SUCCESS) {
         Path_free(oPPath);
         Path_free(oPPrefix);
         if(oNFirstNew != NULL)
            (void) Node_free(oNFirstNew);
         assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));
         return iStatus;
      }

      /* set up for next level */
      Path_free(oPPrefix);
      oNCurr = oNNewNode;
      ulNewNodes++;
      if(oNFirstNew == NULL)
         oNFirstNew = oNCurr;
      ulIndex++;
   }



   Path_free(oPPath);
   /* update DT state variables to reflect insertion */
   if(oNRoot == NULL)
      oNRoot = oNFirstNew;
   ulCount += ulNewNodes;

   assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));
   return SUCCESS;
}

/*Returns TRUE if the FT contains a file with absolute path
  pcPath and FALSE if not or if there is an error while checking.*/
boolean FT_containsFile(const char *pcPath){
    int iStatus;
   Node_T oNFound = NULL;

   assert(pcPath != NULL);

   iStatus = FT_findNode(pcPath, &oNFound);
   /*CHANGED to check if it contains a file instead of a dir*/
   if (iStatus == FALSE && Node_isFile(oNFound) == FALSE){
    return TRUE;
   }
   return (boolean) (iStatus == SUCCESS);
}


/*Removes the FT file with absolute path pcPath.
  Returns SUCCESS if found and removed.
  Otherwise, returns:
  * INITIALIZATION_ERROR if the FT is not in an initialized state
  * BAD_PATH if pcPath does not represent a well-formatted path
  * CONFLICTING_PATH if the root exists but is not a prefix of pcPath
  * NO_SUCH_PATH if absolute path pcPath does not exist in the FT
  * NOT_A_FILE if pcPath is in the FT as a directory not a file
  * MEMORY_ERROR if memory could not be allocated to complete request*/
/*diff = returns NOT_A_FILE*/
int FT_rmFile(const char *pcPath){
    int iStatus;
   Node_T oNFound = NULL;

   assert(pcPath != NULL);
   assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));

   iStatus = DT_findNode(pcPath, &oNFound);

   /*CHANGED added check if this is a directory*/
   if (Node_isFile(oNFound) == FALSE)
        return NOT_A_FILE;

   if(iStatus != SUCCESS)
       return iStatus;

   ulCount -= Node_free(oNFound);
   if(ulCount == 0)
      oNRoot = NULL;

   assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));
   return SUCCESS;
}

/*
  Returns the contents of the file with absolute path pcPath.
  Returns NULL if unable to complete the request for any reason.

  Note: checking for a non-NULL return is not an appropriate
  contains check, because the contents of a file may be NULL.
*/
/*totally new function; calls helper function; recheck later*/
void *FT_getFileContents(const char *pcPath){
    Node_T oNFile = NULL;
    int iStatus; 
    void *nodeContents;

    iStatus = FT_findNode(pcPath, oNFile);
    /*checks if directory*/
    if (Node_isFile(oNFile) == FALSE){
        return NULL;
    }
    if (iStatus == SUCCESS){
        Node_returnContents(Node_T oNFile);
        *nodeContents = oNNode->contents;
        return nodeContents;
    }

    return NULL;
}

/*
  Replaces current contents of the file with absolute path pcPath with
  the parameter pvNewContents of size ulNewLength bytes.
  Returns the old contents if successful. (Note: contents may be NULL.)
  Returns NULL if unable to complete the request for any reason.
*/
void *FT_replaceFileContents(const char *pcPath, void *pvNewContents, size_t ulNewLength){
    Node_T oNFile = NULL;
    int iStatus; 
    void *nodeContents;

    iStatus = FT_findNode(pcPath, oNFile);
    /*checks if directory*/
    if (Node_isFile(oNFile) == FALSE){
        return NULL;
    }
    if (iStatus == SUCCESS){
        Node_returnContents(Node_T oNFile);
        *nodeContents = oNNode->contents;
    }

    /*rewriting without clearing should hopefully work? check later*/
    Node_insertContents(Node_T oNNode, void *pvNewContents, size_t ulNewLength)

    return nodeContents;
}

/*
  Returns SUCCESS if pcPath exists in the hierarchy,
  Otherwise, returns:
  * INITIALIZATION_ERROR if the FT is not in an initialized state
  * BAD_PATH if pcPath does not represent a well-formatted path
  * CONFLICTING_PATH if the root's path is not a prefix of pcPath
  * NO_SUCH_PATH if absolute path pcPath does not exist in the FT
  * MEMORY_ERROR if memory could not be allocated to complete request

  When returning SUCCESS,
  if path is a directory: sets *pbIsFile to FALSE, *pulSize unchanged
  if path is a file: sets *pbIsFile to TRUE, and
                     sets *pulSize to the length of file's contents

  When returning another status, *pbIsFile and *pulSize are unchanged.
*/
/*NOT DONE*/
int FT_stat(const char *pcPath, boolean *pbIsFile, size_t *pulSize);

/*
  Sets the FT data structure to an initialized state.
  The data structure is initially empty.
  Returns INITIALIZATION_ERROR if already initialized,
  and SUCCESS otherwise.
*/
/*should be no different from DT*/
int FT_init(void){
   assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));

   if(bIsInitialized)
      return INITIALIZATION_ERROR;

   bIsInitialized = TRUE;
   oNRoot = NULL;
   ulCount = 0;

   assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));
   return SUCCESS; 
}

/*
  Removes all contents of the data structure and
  returns it to an uninitialized state.
  Returns INITIALIZATION_ERROR if not already initialized,
  and SUCCESS otherwise.
*/
/*should also be no different?*/
int FT_destroy(void){
    assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));

   if(!bIsInitialized)
      return INITIALIZATION_ERROR;

   if(oNRoot) {
      ulCount -= Node_free(oNRoot);
      oNRoot = NULL;
   }

   bIsInitialized = FALSE;

   assert(CheckerDT_isValid(bIsInitialized, oNRoot, ulCount));
   return SUCCESS;
}

/*
  Returns a string representation of the
  data structure, or NULL if the structure is
  not initialized or there is an allocation error.

  The representation is depth-first with files
  before directories at any given level, and nodes
  of the same type ordered lexicographically.

  Allocates memory for the returned string,
  which is then owned by client!
*/
/*loop through twice!!*/
char *FT_toString(void);
