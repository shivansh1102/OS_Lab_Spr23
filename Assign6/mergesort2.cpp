#include <iostream>
#include <cstring>
#include <fstream>
#include <chrono>
#include "goodmalloc.hpp"
using namespace std;

void merge(char* list, uint8_t listLen, uint32_t sp, uint32_t fp, int l1, int r1, int l2, int r2, char* list2, uint8_t list2Len, uint32_t sp2, uint32_t fp2)
{
    newFuncBegin();

    int idx = 0, temp1, temp2;
    while(l1 <= r1 && l2 <= r2)
    {
        temp1 = getVal(list, listLen, l1, sp, fp);
        temp2 = getVal(list, listLen, l2, sp, fp);
        if(temp1 < temp2)
        {
            assignVal(list2, list2Len, idx, temp1, sp2, fp2);
            l1++; 
            idx++;
        }
        else
        {
            assignVal(list2, list2Len, idx, temp2, sp2, fp2);
            l2++; 
            idx++;
        }
    }
    while(l1 <= r1)
    {
        temp1 = getVal(list, listLen, l1, sp, fp);
        assignVal(list2, list2Len, idx, temp1, sp2, fp2);
        l1++;
        idx++;
    }
    while(l2 <= r2)
    {
        temp1 = getVal(list, listLen, l2, sp, fp);
        assignVal(list2, list2Len, idx, temp1, sp2, fp2);
        l2++;
        idx++;
    }

    freeElem();
    FuncEnd();
}

void mergesort(char* listName, uint8_t listNameLen, uint32_t listSize, uint32_t sp, uint32_t fp, int l, int r)
{
    cout << l << " " << r << endl;
    newFuncBegin();
    
    if(l >= r)
    {
        freeElem();
        FuncEnd();
        return;
    }

    uint32_t tmp;
    int mid = (l + r)/2;

    mergesort(listName, listNameLen, listSize, sp, fp, l, mid);
    mergesort(listName, listNameLen, listSize, sp, fp, mid+1, r);

    createList(r-l+1, "temp", 4);
    merge(listName, listNameLen, sp, fp, l, mid, mid+1, r, "temp", 4, getCurStackPointer(), getCurFramePointer());

    for(int i = l; i <= r; i++)
    {
        tmp = getVal("temp", 4, i-l);
        assignVal(listName, listNameLen, i, tmp, sp, fp);
    }

    freeElem();
    FuncEnd();
}

int main()
{
    srand(time(NULL));
    ofstream outFile("output.txt");
    
    const int SIZE = 250*1024*1024, ARRSIZE = 50000;
    auto startTime = chrono::high_resolution_clock().now();

    createMem(SIZE);
    newFuncBegin();

    createList(ARRSIZE, "arr", 3);

    for(int i = 0; i < ARRSIZE; i++)
    assignVal("arr", 3, i, rand()%100000+1);

    outFile << "-------------Before Sorting------------" << endl;

    for(int i = 0; i < ARRSIZE; i++)
    outFile << getVal("arr", 3, i) << " ";
    outFile << endl;

    mergesort("arr", 3, ARRSIZE, getCurStackPointer(), getCurFramePointer(), 0, ARRSIZE-1);

    outFile << "-------------After Sorting------------" << endl;
    for(int i = 0; i < ARRSIZE; i++)
    outFile << getVal("arr", 3, i) << " ";
    outFile << endl;

    freeElem();
    FuncEnd();
    deleteMem();

    auto endTime = chrono::high_resolution_clock().now();
    auto timeTaken = chrono::duration_cast<chrono::milliseconds>(endTime-startTime);
    outFile << "Time taken: " << timeTaken.count() << endl;
    return 0;
}
//638320 567480
