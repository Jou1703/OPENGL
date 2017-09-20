/*******************************************************************************
*** (C) Copyright 2011-Present, Foxconn, Inc. All Rights Reserved.           ***
********************************************************************************
* FILE NAME:    mmiLst.h
*
* FILE PURPOSE: Interface for MMI List utility.
*
* DESCRIPTION:  To provide a simple list function set for other MMI units.
*
********************************************************************************
* HISTORY:
*
*   Author          Date            Reason and Search Flag(Option)
*-------------------------------------------------------------------------------
*   Elvin           2011/09/28      File Creation
*******************************************************************************/
#ifndef __MMILST_H__
#define __MMILST_H__

/*******************************************************************************
*   Include files
*******************************************************************************/

/*******************************************************************************
*   Public Definitions
*******************************************************************************/
#define INFINITE_NODE_CNT 0 
#define BOOL int
#define UINT32 int
#define INT32 int
#define TRUE 1
#define FALSE 0
#define PUBLIC
#define PRIVATE
#define IN
#define OUT
#define mmiOs_mallocMem(size) malloc(size)
#define mmiOs_freeMem(pMemBlock) free(*pMemBlock)

/*******************************************************************************
*   Public Declarations
*******************************************************************************/
/*******************************************************************************
*   Function:       mmiLst_new
********************************************************************************
*   Description:    Create a new list.
*
*   Parameters:     nMaxNodeCnt: The max allowed node count of new list.
*                   bSortById  : The flag indicate whether sort or not by ID 
*                                in this list. TRUE/FALSE means Yes/No.
*
*   Return Values:  The handler of created list.
*******************************************************************************/
PUBLIC void* mmiLst_new(IN UINT32 nMaxNodeCnt, IN BOOL bSortById);

/*******************************************************************************
*   Function:       mmiLst_del
********************************************************************************
*   Description:    Delete a specified list.
*
*   Parameters:     pList: The specified list which will be deleted.
*
*   Return Values:  The result of deletion
*                    0: Successfully.
*                   -1: the list is NULL.
*                   -2: the list is not empty. At least one node in list. 
*******************************************************************************/
PUBLIC INT32 mmiLst_del(IN void** pList);

/*******************************************************************************
*   Function:       mmiLst_addNode
********************************************************************************
*   Description:    Add a new node which includes specified ID & content into a
*                   specified list.
*
*   Parameters:     pList   : The specified list.
*                   nId     : The specified ID.
*                   pContent: The specified content.
*       
*   Return Values:  The result of addition.
*                    0: Successfully.
*                   -1: the list is NULL.
*                   -2: the list is full.
*                   -3: malloc node failed.
*                   -4: add node at tail failed.
*                   -5: add node by sort failed.
*******************************************************************************/
PUBLIC INT32 mmiLst_addNode(IN void* pList, IN UINT32 nId, IN void* pContent);

/*******************************************************************************
*   Function:       mmiLst_delNode
********************************************************************************
*   Description:    Delete a node which includes specified ID from a specified 
*                   list and get its content.
*
*   Parameters:     pList   : The specified list.
*                   nId     : The specified ID.
*                   pContent: (Output) The content of the node. NULL is allowed.
*       
*   Return Values:  The delete result.
*                    0: Successfully.
*                   -1: the list is NULL.
*                   -2: the ID is not found in the list.
*******************************************************************************/
PUBLIC INT32 mmiLst_delNode(IN void* pList, IN UINT32 nId, OUT void** pContent);

/*******************************************************************************
*   Function:       mmiLst_getNode
********************************************************************************
*   Description:    Get a node which includes specified ID from a specified list.
*
*   Parameters:     pList   : The specified list.
*                   nId     : The id of the specified node.
*                   pNode   : (Output) The node includes the specified ID.
*                   pContent: (Output) The content of the node.
*
*   Return Values:  The result of execution.
*                    0: Successfully.
*                   -1: ???
*                   -2: ???
*******************************************************************************/
PUBLIC INT32 mmiLst_getNode(IN void* pList, IN UINT32 nId, OUT void**pNode, OUT void** pContent);

/*******************************************************************************
*   Function:       mmiLst_getNextNode
********************************************************************************
*   Description:    Get the next node of a specified node in a specified list.
*
*   Parameters:     pList    : The specified list.
*                   pNode    : The specified node. NULL means to get tail node.
*                   pNextNode: (Output) The next node.
*                   pContent : (Output) The content of the next node.
*
*   Return Values:  The result of execution.
*                    0: Successfully.
*                   -1: ???
*                   -2: ???
*******************************************************************************/
PUBLIC INT32 mmiLst_getNextNode(IN void* pList, IN void* pNode, OUT void**pNextNode, OUT void** pContent);

/*******************************************************************************
*   Function:       mmiLst_insNextNode
********************************************************************************
*   Description:    
*
*   Parameters:     
*
*   Return Values:  
*
*******************************************************************************/
PUBLIC INT32 mmiLst_insNextNode(IN void* pList, IN void* pNode, IN UINT32 nId, IN void* pContent, OUT void** pInsNode);

/*******************************************************************************
*   Function:       mmiLst_getPrevNode
********************************************************************************
*   Description:    Get the previous node in list.
*
*   Parameters:     pList   : The list which contains the previous node.
*                   pNode   : The current node in list.
*                   pContent: The content of previous node.
*
*   Return Values:  The previous node which get from list.
*
*******************************************************************************/
PUBLIC INT32 mmiLst_getPrevNode(IN void* pList, IN void* pNode, OUT void**pPrevNode, OUT void** pContent);

/*******************************************************************************
*   Function:       mmiLst_insPrevNode
********************************************************************************
*   Description:    
*
*   Parameters:     
*
*   Return Values:  
*
*******************************************************************************/
PUBLIC INT32 mmiLst_insPrevNode(IN void* pList, IN void* pNode, IN UINT32 nId, IN void* pContent, OUT void** pInsNode);

/*******************************************************************************
*   Function:       mmiLst_addNthNode
********************************************************************************
*   Description:    Add the nth node in list.
*
*   Parameters:     pList    : The list which the nth node will be added to.
*                   nNth     : The node location in list.
*                   nId      : The node's id.
*                   pContent : The content of nth node.
*
*   Return Values:  The add result.
*                   -1 means the list is NULL.
*                   -2 means the list is full.
*                   -3 means malloc failed.
*                   0  means add nth node successfully.
*******************************************************************************/
PUBLIC INT32 mmiLst_addNthNode(IN void* pList, IN UINT32 nNth, IN UINT32 nId, IN void* pContent);

/*******************************************************************************
*   Function:       mmiLst_delNthNode
********************************************************************************
*   Description:    Delete the nth node in list.
*
*   Parameters:     pList    : The list which contains the nth node.
*                   nNth     : The node location in list.
*                   pContent : The content of nth node.
*
*   Return Values:  The delete result.
*                   -1 means the list is NULL.
*                   -2 means the nth is more than node count.
*                   0  means delete nth node successfully.
*******************************************************************************/
PUBLIC INT32 mmiLst_delNthNode(IN void* pList, IN UINT32 nNth, OUT void** pContent);

/*******************************************************************************
*   Function:       mmiLst_getNthNode
********************************************************************************
*   Description:    Get the nth node in list. 
*
*   Parameters:     pList    : The list which contains the nth node.
*                   nNth     : The node location in list.
*                   pContent : The content of nth node.
*
*   Return Values:  The nth node in list.
*
*******************************************************************************/
PUBLIC INT32 mmiLst_getNthNode(IN void* pList, IN UINT32 nNth, OUT void**pNode, OUT void** pContent);

/*******************************************************************************
*   Function:       mmiLst_getNodeCnt
********************************************************************************
*   Description:    Get the node count of list. 
*
*   Parameters:     pList: The list which user request to get node count.
*
*   Return Values:  The node count of list.
*
*******************************************************************************/
PUBLIC INT32 mmiLst_getNodeCnt(IN void* pList);

#if 0
PUBLIC void* mmiLst_delNthNode(IN void* pList, IN UINT32 nNth);
PUBLIC INT32 mmiLst_getNodeIdx(IN void* pList, IN UINT32 nId);
PUBLIC void* mmiLst_getNthNode(IN void* pList, IN UINT32 nNth);     /* The node content */
PUBLIC BOOL mmiLst_getNthId(IN void* pList, IN UINT32 nNth, OUT UINT32* pId);
PUBLIC INT32 mmiLst_getNodeCnt(IN void* pList);
PUBLIC void* mmiLst_getNextNode(IN void* pList, IN void** pNode, IN BOOL bFirst);
PUBLIC void* mmiLst_getPrevNode(IN void* pList, IN void** pNode, IN BOOL bLast);
PUBLIC void* mmiLst_getFirstNode(IN void* pList);
PUBLIC void* mmiLst_getLastNode(IN void* pList);
PUBLIC BOOL mmiLst_insertNode(IN void *pList, IN UINT32 nId, IN UINT32 nPrevId, IN void *pContent);
PUBLIC BOOL mmiLst_insertPrvNode(IN void *pList, IN UINT32 nId, IN UINT32 nPrevId, IN void *pContent, IN BOOL blBefore);
PUBLIC BOOL mmiLst_moveToLastNode(IN void *pList, IN void *pNode);
PUBLIC BOOL mmiLst_moveNode(IN void *pList, IN UINT32 nId, IN UINT32 nPrevId, IN BOOL bIsBehind);
#endif

#endif  /* __MMILST_H__ */

