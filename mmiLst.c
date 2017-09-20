/*******************************************************************************
*** (C) Copyright 2011-Present, Foxconn, Inc. All Rights Reserved.           ***
********************************************************************************
* FILE NAME:    mmiLst.c
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

/*******************************************************************************
*   Include files
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mmiLst.h"

/*******************************************************************************
*   Public Definitions
*******************************************************************************/
#define MMILST_MAX_NODE_CNT 65535

/*******************************************************************************
*   Private Definitions
*******************************************************************************/
typedef struct S_mmiNode
{
    UINT32              nId;
    void*               pContent;
    struct S_mmiNode*   pNextNode;
    struct S_mmiNode*   pPrevNode;
}T_S_mmiNode;

typedef struct S_mmiLst
{
    UINT32          nMaxNodeCnt;    /* max count of node allowed in this list */
    BOOL            bSortById;      /* auto-sort by node's id */
    UINT32          nCurNodeCnt;    /* current count of nodes */
    T_S_mmiNode*    pHeadNode;      /* the first node */
    T_S_mmiNode*    pTailNode;      /* the last node */
}T_S_mmiLst;

PRIVATE BOOL addNodeAtTail(IN T_S_mmiLst* pMmiLst, IN T_S_mmiNode* pAddNode);
PRIVATE T_S_mmiNode* seekLstNode(IN void* pList, IN UINT32 nId);
PRIVATE BOOL addNodeBySort(IN T_S_mmiLst* pMmiLst, IN T_S_mmiNode* pAddNode);

/*******************************************************************************
*   Private Variables
*******************************************************************************/

/*******************************************************************************
*   Private Routines
*******************************************************************************/
/*******************************************************************************
* FUNCTION NAME:    addNodeAtTail
********************************************************************************
* FUNCTION PURPOSE: Add the input node to the tail of the input list.
* PARAMETER:        pMmiLst    --  the input list.
*                   pAddNode   --  the input node.
* RETURN VALUE:     TRUE       --  successful.
*                   FALSE      --  failed.
*******************************************************************************/
PRIVATE BOOL addNodeAtTail(IN T_S_mmiLst *pMmiLst, IN T_S_mmiNode *pAddNode)
{
    /* add into a empty list */
    if(pMmiLst->nCurNodeCnt == 0)
    {
        pMmiLst->pHeadNode = pAddNode;
    }
    else
    {
        pMmiLst->pTailNode->pNextNode = pAddNode;
        pAddNode->pPrevNode = pMmiLst->pTailNode;
    }
    pMmiLst->pTailNode = pAddNode;
    pMmiLst->nCurNodeCnt++;
    
    return TRUE;
}
/*******************************************************************************
* FUNCTION NAME:    addNodeBySort
********************************************************************************
* FUNCTION PURPOSE: Add the input node into the input list.
* PARAMETER:        pMmiLst    --  the input list.
*                   pAddNode   --  the input node.
* RETURN VALUE:     TRUE       --  successful.
*                   FALSE      --  failed.
*******************************************************************************/
PRIVATE BOOL addNodeBySort(IN T_S_mmiLst *pMmiLst,IN T_S_mmiNode *pAddNode)
{
    BOOL        bRet      = FALSE;
    T_S_mmiNode *pCurNode = pMmiLst->pHeadNode;
    T_S_mmiNode *pPreNode = pMmiLst->pHeadNode;

    if(pMmiLst->nCurNodeCnt == 0)   //While mmiLst is empty. 
    {
        pMmiLst->pHeadNode = pAddNode;
        pMmiLst->pTailNode = pAddNode;
        bRet = TRUE;
    }
    else
    {
        while(pCurNode != NULL)
        {
            if(pAddNode->nId > pCurNode->nId)
            {
                pPreNode = pCurNode;
                pCurNode = pCurNode->pNextNode;
            }
            else
            {
                if(pCurNode == pMmiLst->pHeadNode)
                {
                    pAddNode->pNextNode = pCurNode;
                    pCurNode->pPrevNode = pAddNode;
                    pMmiLst->pHeadNode = pAddNode;
                    bRet = TRUE;
                    break;
                }
                else
                {   
                    pAddNode->pNextNode = pCurNode;
                    pAddNode->pPrevNode = pPreNode;
                    pPreNode->pNextNode = pAddNode;
                    pCurNode->pPrevNode = pAddNode;
                    bRet = TRUE;
                    break;
                }
            }
            if(pCurNode == NULL)
            {
                pPreNode->pNextNode = pAddNode;
                pAddNode->pPrevNode = pPreNode;
                pMmiLst->pTailNode = pAddNode;
                bRet = TRUE;
                break;
            }
        }
    }
    if(bRet == TRUE)
        pMmiLst->nCurNodeCnt++;

	return bRet;
}
/*******************************************************************************
* FUNCTION NAME:    seekLstNode
********************************************************************************
* FUNCTION PURPOSE: Find the input node by index in input list.
* PARAMETER:        pList   -- the seek list.
*                   nId     -- the keyword of the node.
* RETURN VALUE:     The node found by index.
*******************************************************************************/
PRIVATE T_S_mmiNode* seekLstNode(IN void* pList, IN UINT32  nId)
{
    T_S_mmiLst  *pMmiLst    = (T_S_mmiLst*)pList;
    T_S_mmiNode *pPtrNode   = NULL;

    pPtrNode = pMmiLst->pHeadNode;
    
    while(pPtrNode != NULL)
    {
        if(pPtrNode->nId == nId)
        {
            return pPtrNode;
        }
        pPtrNode = pPtrNode->pNextNode;
    }  
    return NULL;
}

/*******************************************************************************
*   Public Declarations
*******************************************************************************/
/*******************************************************************************
*   Function:       mmiLst_new
********************************************************************************
*   Description:    Create a new list.
*
*   Parameters:     nMaxNodeCnt: The max allowed node count of new list. The
*                                default value is MMILST_MAX_NODE_CNT.
*                   bSortById  : The flag indicate whether sort or not by ID 
*                                in this list. TRUE/FALSE means Yes/No. The 
*                                default value is FALSE. 
*
*   Return Values:  The handler of created list.
*******************************************************************************/
PUBLIC void* mmiLst_new(IN UINT32  nMaxNodeCnt, IN BOOL bSortById)
{
    T_S_mmiLst *pMmiLst = (T_S_mmiLst*)mmiOs_mallocMem(sizeof(T_S_mmiLst));
    
    if(pMmiLst == NULL)
        return NULL;

    if((nMaxNodeCnt <= INFINITE_NODE_CNT) || (nMaxNodeCnt>MMILST_MAX_NODE_CNT))
        nMaxNodeCnt = MMILST_MAX_NODE_CNT;
    
 	pMmiLst->nMaxNodeCnt = nMaxNodeCnt;       
    pMmiLst->nCurNodeCnt = 0;
    pMmiLst->bSortById = bSortById;
    pMmiLst->pHeadNode = NULL;
    pMmiLst->pTailNode = NULL;
    
    return (void*)pMmiLst;            
}

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
PUBLIC INT32  mmiLst_del(IN void** pList)
{
	T_S_mmiLst *pMmiLst = (T_S_mmiLst*)*pList;
    
    if(pMmiLst == NULL)
        return -1;

    if(pMmiLst->nCurNodeCnt > 0)
        return -2;
    /* delete list */
    mmiOs_freeMem(&pMmiLst);
    *pList = NULL;
    
    return 0;	
}

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
PUBLIC INT32 mmiLst_addNode(IN void* pList, IN UINT32  nId, IN void* pContent)
{
	T_S_mmiNode *pNewNode = NULL;
    T_S_mmiLst  *pMmiLst  = (T_S_mmiLst*)pList;
    
    if(pMmiLst == NULL)
        return -1;
    
    if(pMmiLst->nCurNodeCnt == pMmiLst->nMaxNodeCnt)
        return -2;

    pNewNode = (T_S_mmiNode*)mmiOs_mallocMem(sizeof(T_S_mmiNode));
    if(pNewNode == NULL)
        return -3;
    
    pNewNode->pContent  = pContent;
    pNewNode->nId       = nId;
    pNewNode->pNextNode = NULL;
    pNewNode->pPrevNode = NULL;
    
    if(pMmiLst->bSortById == FALSE)
    {
        if(addNodeAtTail(pMmiLst, pNewNode) == FALSE)
        {
            mmiOs_freeMem(&pNewNode);
            return -4;
        }
    }
    else
    {
        if(addNodeBySort(pMmiLst, pNewNode) == FALSE)
        {
            mmiOs_freeMem(&pNewNode);
            return -5;
        }
    }
    return 0;
}

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
*   Return Values:  The result deletion.
*                    0: Successfully.
*                   -1: the list is NULL.
*                   -2: the ID is not found in the list.
*******************************************************************************/
PUBLIC INT32  mmiLst_delNode(IN void* pList, IN UINT32  nId, OUT void** pContent)
{
	T_S_mmiLst  *pMmiLst     = (T_S_mmiLst *)pList;
    T_S_mmiNode *pDelNode    = NULL;
    /* check input list & node */
    if(pMmiLst == NULL)
        return -1;

    /* seek node */
    if((pDelNode = seekLstNode(pList, nId)) == NULL)
        return -2;
    /* check previous node */
    if(pDelNode->pPrevNode == NULL)
    {
        pMmiLst->pHeadNode = pDelNode->pNextNode;
    }
    else
    {
        pDelNode->pPrevNode->pNextNode = pDelNode->pNextNode;
    }

    /* check next node */
    if(pDelNode->pNextNode == NULL)
    {
        pMmiLst->pTailNode = pDelNode->pPrevNode;
    }
    else
    {
        pDelNode->pNextNode->pPrevNode = pDelNode->pPrevNode;
    }
    *pContent = pDelNode->pContent;
    mmiOs_freeMem(&pDelNode);
    pDelNode = NULL;
    pMmiLst->nCurNodeCnt--;
    
    return 0;	
}

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
*                   -1: -1 means the list is NULL.
*                   -2: the ID is not found in the list
*******************************************************************************/
PUBLIC INT32  mmiLst_getNode(IN void* pList, IN UINT32  nId,OUT void **pNode,OUT void** pContent)
{
	T_S_mmiLst  *pMmiLst = (T_S_mmiLst*)pList;
    T_S_mmiNode *pTempNode = NULL;
    /* check input list & node */
    if(pMmiLst == NULL)
        return -1;
    pTempNode = seekLstNode(pMmiLst, nId);

    if(pTempNode == NULL)
        return -2;

    *pNode = (void *)pTempNode;
    *pContent = pTempNode->pContent; 
    return  0;	
}

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
*                   -1: -1 means the list or pNode is NULL.
*                   -2: -2 means the specified node isn't in the list.
*******************************************************************************/
PUBLIC INT32  mmiLst_getNextNode( IN void* pList, IN void* pNode, OUT void** pNextNode,OUT void** pContent)
{
    T_S_mmiLst  *pMmiLst   = (T_S_mmiLst*)pList;
    T_S_mmiNode *pCurNode  = (T_S_mmiNode *)pNode;
    T_S_mmiNode *pTempNode = NULL;
    
    if(pMmiLst == NULL)
        return -1;   

    if(pCurNode == NULL)
        pTempNode = pMmiLst->pTailNode;
    else
    {
        if(pCurNode != seekLstNode(pMmiLst, pCurNode->nId))
            return -2; 
        pTempNode = pCurNode->pNextNode;	
    }
    
    if(pTempNode == NULL)
        *pNextNode = NULL;
    else
    {
        *pNextNode = (void *)pTempNode;
        *pContent  = pTempNode->pContent;
    }

    return 0;
}

/*******************************************************************************
*   Function:       mmiLst_insNextNode
********************************************************************************
*   Description: Insert node to next of a specified node in a specified list.   
*
*   Parameters:     pList    : The specified list.
*                   pNode    : The specified node.
*                   nId      : The nId of the insert node.
*                   pContent : The content of the insert node. 
*                   pInsNode : (Output)The insert node.
*
*   Return Values:  The result of execution.
*                    0: Successfully.
*                   -1: -1 means the list or pNode is NULL.
*                   -2: -2 means the specified node isn't in the specified list.
*                   -3: -3 means the list is full.  
*                   -4: -4 means nId is invalid.
                    -5: -5 means malloc node failed.
*
*******************************************************************************/
PUBLIC INT32  mmiLst_insNextNode(IN void* pList, IN void* pNode, IN UINT32  nId, IN void* pContent, OUT void** pInsNode)
{
    T_S_mmiLst  *pMmiLst = (T_S_mmiLst*)pList;
    T_S_mmiNode *pTempNode = NULL;
    T_S_mmiNode *pCurNode = (T_S_mmiNode *)pNode;
        
    if(pMmiLst == NULL || pNode == NULL)
        return -1;
    
    if(pCurNode != seekLstNode(pMmiLst, pCurNode->nId))
        return -2; 
    
    if(pMmiLst->nCurNodeCnt == pMmiLst->nMaxNodeCnt)
        return -3;

    if(pMmiLst->bSortById == TRUE)
    {
        if(pCurNode->nId > nId)
            return -4;
        if((pCurNode->pNextNode != NULL) && nId > (pCurNode->pNextNode->nId))
            return -4;
    }
       
    pTempNode = (T_S_mmiNode*)mmiOs_mallocMem(sizeof(T_S_mmiNode));
    /* insert new node into list follow the prev node */
    if(pTempNode == NULL)
        return -5;
    
    pTempNode->pContent  = pContent;
    pTempNode->nId       = nId;
    pTempNode->pNextNode = pCurNode->pNextNode;
    pTempNode->pPrevNode = pCurNode;
    
    if(pCurNode->pNextNode == NULL)
        pMmiLst->pTailNode = pTempNode;
    else
        pCurNode->pNextNode->pPrevNode = pTempNode;
    
    pCurNode->pNextNode = pTempNode;
    pMmiLst->nCurNodeCnt++;
    *pInsNode = (void *)pTempNode;
    return 0;
}
/*******************************************************************************
*   Function:       mmiLst_getPrevNode
********************************************************************************
*   Description:    Get the previous node in list.
*
*   Parameters:     pList   : The list which contains the previous node.
*                   pNode   :  The specified node. NULL means to get head node.
*                   pPrevNode : (Output)The previous node.
*                   pContent  : (Output)The content of previous node.
*
*   Return Values:  The result of execution.
*                    0: Successfully.
*                   -1: -1 means the list is NULL.
*                   -2: -2 means the specified node isn't in the list.
*
*******************************************************************************/
PUBLIC UINT32  mmiLst_getPrevNode(IN void* pList, IN void* pNode, OUT void**pPrevNode, OUT void** pContent)
{
	T_S_mmiLst  *pMmiLst    = (T_S_mmiLst*)pList;
    T_S_mmiNode *pCurNode   = (T_S_mmiNode *)pNode;
    T_S_mmiNode *pTempNode  = NULL;

    if(pMmiLst == NULL)
        return -1;
    
    if(pCurNode == NULL)
        pTempNode = pMmiLst->pHeadNode;
    else
    {
        if(pCurNode != seekLstNode(pMmiLst, pCurNode->nId))
            return -2;  
        pTempNode = pCurNode->pPrevNode;
    }
    
    if(pTempNode == NULL)
        *pPrevNode = NULL;   
    else
    {
        *pPrevNode = (void *)pTempNode;
        *pContent = pTempNode->pContent;
    }

    return 0;
}

/*******************************************************************************
*   Function:       mmiLst_insPrevNode
********************************************************************************
*   Description: Insert node to previous of a specified node in a specified list.
*   Parameters:     pList    : The specified list.
*                   pNode    : The specified node.
*                   nId      : The nId of the insert node.
*                   pContent : The content of the insert node. 
*                   pInsNode : (Output)The insert node.
*
*   Return Values:  The result of execution.
*                    0: Successfully.
*                   -1: -1 means the list or pNode is NULL.
*                   -2: -2 means the specified node isn't in the list.
*                   -3: -3 means the list is full.  
*                   -4: -4 means nId is invalid when list is sorted.
*                   -5: -5 means malloc node failed.
*
*******************************************************************************/
PUBLIC INT32  mmiLst_insPrevNode(IN void* pList, IN void* pNode, IN UINT32  nId,IN void* pContent, OUT void** pInsNode)
{
	T_S_mmiLst  *pMmiLst = (T_S_mmiLst*)pList;
    T_S_mmiNode *pCurNode = (T_S_mmiNode *)pNode;
    T_S_mmiNode *pTempNode = NULL;
        
    if(pMmiLst == NULL || pNode == NULL)
        return -1;
    
    if(pCurNode != seekLstNode(pMmiLst, pCurNode->nId))
        return -2;    
    
    if(pMmiLst->nCurNodeCnt == pMmiLst->nMaxNodeCnt)
        return -3;
    
    if(pMmiLst->bSortById == TRUE)
    {
        if(pCurNode->nId < nId)
            return -4;
        if((pCurNode->pPrevNode != NULL) && nId < (pCurNode->pPrevNode->nId))
            return -4;
    }

    pTempNode = (T_S_mmiNode*)mmiOs_mallocMem(sizeof(T_S_mmiNode));
    /* insert new node into list follow the prev node */
    if(pTempNode==NULL)
        return -5;
    
    pTempNode->pContent  = pContent;
    pTempNode->nId       = nId;
    pTempNode->pPrevNode = pCurNode->pPrevNode;
    pTempNode->pNextNode = pCurNode;
    
    if(pCurNode->pPrevNode == NULL)
        pMmiLst->pHeadNode = pTempNode;
    else
        pCurNode->pPrevNode->pNextNode = pTempNode;   

    pCurNode->pPrevNode = pTempNode; 
    pMmiLst->nCurNodeCnt++;
    *pInsNode = (void *)pTempNode;
    return 0;	
}

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
*                    0 means add nth node successfully.
*                   -1 means the list is NULL.
*                   -2 means the list is full.
*                   -3 means nNth is invalid.
*                   -4 means malloc failed.
*                   -5 means nId is invalid while list is sorted.
*                   -6 means add node to tail of list failed.
*                   -7 means the other exceptions.
*******************************************************************************/
PUBLIC INT32  mmiLst_addNthNode(IN void* pList, IN UINT32  nNth, IN UINT32  nId, IN void* pContent)
{
	T_S_mmiNode *pNewNode = NULL;
	T_S_mmiNode *pNode = NULL;
    T_S_mmiLst  *pMmiLst = (T_S_mmiLst*)pList;
    UINT32       nIdx     = 1;
    
    if(pMmiLst == NULL)
        return -1;
    
    if(pMmiLst->nCurNodeCnt == pMmiLst->nMaxNodeCnt)
        return -2;

    if(nNth<1 || (nNth > pMmiLst->nCurNodeCnt+1))
         return -3;
    
    pNewNode = (T_S_mmiNode*)mmiOs_mallocMem(sizeof(T_S_mmiNode));
    if(pNewNode==NULL)
        return -4;
     
    pNewNode->pContent  = pContent;
    pNewNode->nId       = nId;
    pNewNode->pNextNode = NULL;
    pNewNode->pPrevNode = NULL;
    
    pNode = pMmiLst->pHeadNode;

    if(pNode == NULL)   //List is empty
    {
        pMmiLst->pHeadNode = pNewNode;
        pMmiLst->pTailNode = pNewNode;
        pMmiLst->nCurNodeCnt++;
        return 0;
    }
    
    if(pMmiLst->bSortById == TRUE)
    {
        if(nNth == pMmiLst->nCurNodeCnt+1)  //If nNth == nCurNodeCnt+1,add node at tail
        {
            if(pNewNode->nId < pMmiLst->pTailNode->nId)
            {
                mmiOs_freeMem(&pNewNode);
                return -5;
            }
            else if(addNodeAtTail(pMmiLst, pNewNode) == TRUE)
                return 0;
            else
            {
                mmiOs_freeMem(&pNewNode);
                return -6;
            }
        }
        while(pNode != NULL)
        {
        	if(nIdx == nNth)
        	{
        	    if(pNewNode->nId > pNode->nId)
                {
                    mmiOs_freeMem(&pNewNode);
                    return -5;
                }
                
        		pNewNode->pPrevNode = pNode->pPrevNode;
        		pNewNode->pNextNode = pNode;
        		if(pNode->pPrevNode != NULL)
                {
                    if(pNode->pPrevNode->nId > pNewNode->nId)
                    {
                        mmiOs_freeMem(&pNewNode);
                        return -5;
                    }
                    pNode->pPrevNode->pNextNode = pNewNode;
                }      
                else
            		pMmiLst->pHeadNode= pNewNode;
            	
        		pNode->pPrevNode = pNewNode;
        		pMmiLst->nCurNodeCnt++;
        		return 0;
        	}
        	pNode = pNode->pNextNode;
        	nIdx++;
        }
    }
    
    else
    {
        if(nNth == pMmiLst->nCurNodeCnt+1)  //If nNth == nCurNodeCnt+1,add node at tail
        {
            if(addNodeAtTail(pMmiLst, pNewNode) == TRUE)
                return 0;
            else
            {
                mmiOs_freeMem(&pNewNode);
                return -6;
            }
        }
        while(pNode != NULL)
        {
        	if(nIdx == nNth)
        	{
        		pNewNode->pPrevNode = pNode->pPrevNode;
        		pNewNode->pNextNode = pNode;
        		if(pNode->pPrevNode != NULL)
            		pNode->pPrevNode->pNextNode = pNewNode;
        		else
            		pMmiLst->pHeadNode= pNewNode;
                
        		pNode->pPrevNode = pNewNode;
        		pMmiLst->nCurNodeCnt++;
        		return 0;
        	}
        	pNode = pNode->pNextNode;
        	nIdx++;
        }
    }
    return -7;	
}

/*******************************************************************************
*   Function:       mmiLst_delNthNode
********************************************************************************
*   Description:    Delete the nth node in list.
*
*   Parameters:     pList    : The list which contains the nth node.
*                   nNth     : The node location in list.
*                   pContent : (Output)The content of nth node.
*
*   Return Values:  The delete result.
*                    0 means delete nth node successful.
*                   -1 means the list is NULL.
*                   -2 means the nth is invalid.
*                   -3 the other exceptions.
*******************************************************************************/
PUBLIC INT32  mmiLst_delNthNode(IN void* pList, IN UINT32  nNth, OUT void** pContent)
{
	T_S_mmiLst  *pMmiLst     = (T_S_mmiLst*)pList;
    T_S_mmiNode *pDelNode    = NULL;
    UINT32       nIdx         = 1;
    
    /* check input list & node */
    if(pMmiLst == NULL)
        return -1;
        
    if(nNth<1 || (nNth > pMmiLst->nCurNodeCnt))
    	return -2;
  
    pDelNode = pMmiLst->pHeadNode;
	
	while(pDelNode != NULL)
    {
    	if(nIdx == nNth)
    	{
    		/* check previous node */
		    if(pDelNode->pPrevNode == NULL)
		    {
		        pMmiLst->pHeadNode = pDelNode->pNextNode;
		    }
		    else
		    {
		        pDelNode->pPrevNode->pNextNode = pDelNode->pNextNode;
		    }
		
		    /* check next node */
		    if(pDelNode->pNextNode == NULL)
		    {
		        pMmiLst->pTailNode = pDelNode->pPrevNode;
		    }
		    else
		    {
		        pDelNode->pNextNode->pPrevNode = pDelNode->pPrevNode;
		    }
		    *pContent = pDelNode->pContent;
		    mmiOs_freeMem(&pDelNode);
		    pDelNode = NULL;
		    pMmiLst->nCurNodeCnt--;
		    return 0;
    	}
    	pDelNode = pDelNode->pNextNode;
    	nIdx++;
    }
    return -3;
}

/*******************************************************************************
*   Function:       mmiLst_getNthNode
********************************************************************************
*   Description:    Get the nth node in list. 
*
*   Parameters:     pList    : The list which contains the nth node.
*                   nNth     : The node location in list.
*                   pNode    : (Output)The nth node
*                   pContent : (Output)The content of nth node.
*
*   Return Values:  The result of get nth node.
*                    0: Successfully.
*                   -1: -1 means the list is NULL.
*                   -2: -2 means nNth is invalid.
*                   -3: -3 means other exceptions.
*
*******************************************************************************/
PUBLIC INT32  mmiLst_getNthNode(IN void* pList, IN UINT32  nNth, OUT void**pNode, OUT void** pContent)
{
	T_S_mmiLst  *pMmiLst     = (T_S_mmiLst*)pList;
    UINT32       nIdx         = 1;
    T_S_mmiNode *pCurNode = NULL;
    
    /* check input list & node */
    if(pMmiLst == NULL)
        return -1;
        
    if(nNth<1 || (nNth > pMmiLst->nCurNodeCnt))
    	return -2;
  
    pCurNode = pMmiLst->pHeadNode;
	
	while(pCurNode != NULL)
    {
    	if(nIdx == nNth)
    	{	
    	    *pContent = pCurNode->pContent;
            *pNode = (void *)pCurNode;
    	    return 0;
    	}
    	pCurNode = pCurNode->pNextNode;
    	nIdx++;
    }
    return -3;
}

/*******************************************************************************
*   Function:       mmiLst_getnCurNodeCnt
********************************************************************************
*   Description:    Get the node count of list. 
*
*   Parameters:     pList: The list which user request to get node count.
*
*   Return Values:  The node count of list.
*
*******************************************************************************/
PUBLIC INT32  mmiLst_getnCurNodeCnt(IN void* pList)
{
	T_S_mmiLst  *pMmiLst     = (T_S_mmiLst*)pList;
    
	if(pMmiLst == NULL)
		return -1;
	return pMmiLst->nCurNodeCnt;
}

