#include <stdio.h>
#include <stdlib.h>
#include "mmiLst.c"
//This is test code. 
void mmiLst_print(void *pList)
{   
    T_S_mmiLst *pMmiLst = (T_S_mmiLst *)pList;
    T_S_mmiNode *pNode = NULL;
    if (pMmiLst == NULL)
        return;
    pNode = pMmiLst->pHeadNode;
    while(pNode != NULL)
    {
        printf("nId= %d.\n",pNode->nId);
        pNode = pNode->pNextNode;
    }
}

void *mmiLst_addTest(int nMaxNodeCnt,BOOL bSortById)
{
    void* pMmiList = mmiLst_new(nMaxNodeCnt,bSortById);
    char *contenStr1 = "hello";
    char *contenStr2 = "world";
    char *contenStr3 = "victor";
    char *contenStr4 = "Test";

    if(pMmiList == NULL)
        return NULL;
    
    mmiLst_addNode(pMmiList,24,contenStr1);
    mmiLst_addNode(pMmiList,22,contenStr2);
    mmiLst_addNode(pMmiList,20,contenStr3);
    mmiLst_addNthNode(pMmiList,4,29,contenStr4);
    return pMmiList;
}

int mmiLst_getinsTest(void *pList)
{
    void *pNode1 = NULL;
    void *pNode2 = NULL;
    void *pNode3 = NULL;
    void *pNode4 = NULL;
    void *pNode5 = NULL;
    void *pContent1 = NULL;
    void *pContent2 = NULL;
    void *pContent3 = NULL;
    char *pContent4 = "How";
    char *pContent5 = "You";
    
    if(mmiLst_getNode(pList,22,&pNode1,&pContent1) !=0)
        return -1;
    if(mmiLst_getNextNode(pList, pNode1, &pNode2,&pContent2) != 0)
        return -2;
    if(mmiLst_getPrevNode(pList, pNode1, &pNode3,&pContent3) != 0)
        return -3;
    printf("In mmiLst_getinsTest(), pContent1:%s, pContent2:%s, pContent3:%s.\n",(char *)pContent1,(char *)pContent2,(char *)pContent3);

    if(mmiLst_insNextNode(pList,pNode1,23,(void*)pContent4,&pNode4) != 0)
        return -4;
    if(mmiLst_insPrevNode(pList,pNode1,21,(void*)pContent5,&pNode5) != 0)
        return -5;
    
    return 0;
}

int mmiLst_delTest(void **pList)
{
    T_S_mmiLst *pMmiLst = (T_S_mmiLst *)*pList;
    T_S_mmiNode *pNode = NULL;
    void* pContent = NULL;

    if(pMmiLst == NULL)
        return -1;
    while(pMmiLst->pHeadNode != NULL)
    {
        mmiLst_delNthNode((void *)pMmiLst,1,&pContent);
    }
    return mmiLst_del(pList);
}
    
int main()
{
    void* pContent = NULL;
    void* pMmiList = mmiLst_addTest(200,TRUE);
    mmiLst_print(pMmiList);
    mmiLst_getinsTest(pMmiList);
    mmiLst_delNode(pMmiList,22,&pContent);
    mmiLst_print(pMmiList);

    printf("The CurNodeCnt of list is %d.\n",mmiLst_getnCurNodeCnt(pMmiList));
    mmiLst_delTest(&pMmiList);
    printf("The CurNodeCnt of list is %d.\n",mmiLst_getnCurNodeCnt(pMmiList));
    
    return 0;
}
