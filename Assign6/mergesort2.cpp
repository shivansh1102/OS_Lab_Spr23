#include <iostream>
#include <cstring>
#include <fstream>
#include "goodmalloc.hpp"
using namespace std;

ofstream outFile("output.txt");

void merge(char* list, uint8_t listLen, int l1, int r1, int l2, int r2, char* list2, uint8_t list2Len)
{
    newFuncBegin();

    int idx = 0, temp1, temp2;
    while(l1 <= r1 && l2 <= r2)
    {
        temp1 = getVal(list, listLen, l1, 1);
        temp2 = getVal(list, listLen, l2, 1);
        if(temp1 < temp2)
        {
            assignVal(list2, list2Len, idx, temp1, 1);
            l1++; 
            idx++;
        }
        else
        {
            assignVal(list2, list2Len, idx, temp2, 1);
            l2++; 
            idx++;
        }
    }
    while(l1 <= r1)
    {
        temp1 = getVal(list, listLen, l1, 1);
        assignVal(list2, list2Len, idx, temp1, 1);
        l1++;
        idx++;
    }
    while(l2 <= r2)
    {
        temp1 = getVal(list, listLen, l2, 1);
        assignVal(list2, list2Len, idx, temp1, 1);
        l2++;
        idx++;
    }

    freeElem();
    FuncEnd();
}

void mergesort(char* listName, uint8_t listNameLen, uint32_t listSize, int l, int r)
{
    newFuncBegin();
    
    outFile << l << " " << r << " " << listSize << endl;
    if(l >= r)
    {
        freeElem();
        FuncEnd();
        return;
    }

    uint32_t temp;
    // First copying the list stored in previous frame, to this frame
    createList(listSize, listName, listNameLen);
    
    for(uint32_t i = 0; i < listSize; i++)
    {
        temp = getVal(listName, listNameLen, i, 1);
        assignVal(listName, listNameLen, i, temp, 0);
        outFile << temp << " ";
    }
    outFile << endl;
    cout << "yes" << endl;
    int mid = (l + r)/2, tmp;

    mergesort(listName, listNameLen, listSize, l, mid);
    mergesort(listName, listNameLen, listSize, mid+1, r);

    createList(r-l+1, "temp", 4);
    merge(listName, listNameLen, l, mid, mid+1, r, "temp", 5);

    for(int i = l; i <= r; i++)
    {
        tmp = getVal("temp", 4, i-l, 0);
        assignVal(listName, listNameLen, i, tmp, 0);
    }

    freeElem();
    FuncEnd();
}

int main()
{
    srand(time(NULL));
    
    const int SIZE = 250*1024*1024, ARRSIZE = 10;
    createMem(SIZE);
    newFuncBegin();

    createList(ARRSIZE, "arr", 3);

    for(int i = 0; i < ARRSIZE; i++)
    assignVal("arr", 3, i, rand()%100000+1, 0);

    for(int i = 0; i < ARRSIZE; i++)
    outFile << getVal("arr", 3, i, 0) << " ";
    outFile << endl;

    mergesort("arr", 3, ARRSIZE, 0, ARRSIZE-1);

    for(int i = 0; i < ARRSIZE; i++)
    outFile << getVal("arr", 3, i, 0) << " ";
    outFile << endl;

    freeElem();
    FuncEnd();
    return 0;
}