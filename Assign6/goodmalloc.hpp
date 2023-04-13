#ifndef __GOOD_MALLOC_HPP
#define __GOOD_MALLOC_HPP

#include <iostream>
#include <cstring>
using namespace std;

const int MAX_FRAME_CNT = 10000;
const int MAX_PAGE_TABLE_ENTRIES = 16384;   // 2^15

char* startBuffer;                      // to store malloced segment header        
uint32_t startMem, endMem;              // to store offset of starting and ending portion of mem
uint32_t totalBytesMalloced;


struct stackEntry
{
    uint16_t pageTableIdx;              // stores index of page table entry corresponding to this
    char* listName;                     // Name of list 
    uint8_t listNameLen;                // assuming name of list can fit in 8 bits 
    stackEntry()
    {
        pageTableIdx = MAX_PAGE_TABLE_ENTRIES;
        listName = NULL;
        listNameLen = 0;
    }
};

class FrameStack
{
    uint32_t fptrs[MAX_FRAME_CNT];           // array implementation of frame pointer stack 
    uint32_t currIdx;                        // current index in fptrs[]
    public:
    FrameStack();
    void push(const uint32_t&);           
    void pop();
    uint32_t top();
    friend class Stack;                     // so that Stack class can access its private data members
};

class Stack
{
    uint32_t sp;                            // stack pointer
    FrameStack fstack;                      // stack of frame pointers
    public:
    Stack(uint32_t);
    void newFuncBeginUtil();                    // Save the current stack pointer as frame pointer in frame stack
    void FuncEndUtil();                         // Pop frame pointer entry
    void push(const stackEntry &);          // Push stack entry 
    stackEntry top();                       // Returns top entry from stack
    void pop();                             // Pop stack entry
    uint16_t findPTidxByName(char* listName, uint8_t listNameLen)
    {
        cout << fstack.top() << endl;
        int currsp = sp;
        char* temp = startBuffer+sp;
        uint8_t lNLen;
        while(currsp > fstack.top())
        {
            lNLen = *(reinterpret_cast<uint8_t*>(temp));
            temp += 1;
            if(listNameLen == lNLen && strncpy(temp, listName, listNameLen))
            {
                temp += lNLen;
                cout << "Page table index found in current stack: " << *reinterpret_cast<uint16_t*>(temp) << endl;
                return *reinterpret_cast<uint16_t*>(temp);
            }
            temp += lNLen + 2;
            currsp += 1 + lNLen + 2;
        }
        return MAX_PAGE_TABLE_ENTRIES;
    }
    uint16_t freeLastListCurrStack()
    {
        if(sp == fstack.top())
        return MAX_PAGE_TABLE_ENTRIES;
        stackEntry se = top();
        pop();
        return se.pageTableIdx;
    }
    // friend class PageTable;
};

struct pageTableEntry
{
    uint32_t headOffset;                    // stores offset of head of list corresponding to this entry if isValid=1
    u_int nextFreeIdx : 15;                 // stores index of next entry in PageTable having isValid=0 
    u_int isValid : 1;                      // isValid = 0 => chunk is free
    pageTableEntry()
    {
        headOffset = 0;
        nextFreeIdx = MAX_PAGE_TABLE_ENTRIES;
        isValid = 0;
    }
};  

class PageTable
{
    pageTableEntry ptearr[MAX_PAGE_TABLE_ENTRIES];
    uint16_t firstFreeIdx;
    uint16_t lastFreeIdx;
    uint16_t currSize;
    public:
    PageTable() : firstFreeIdx(0), lastFreeIdx(0), currSize(0)
    {
        ptearr[currSize].headOffset = sizeof(PageTable);
        ptearr[currSize].nextFreeIdx = MAX_PAGE_TABLE_ENTRIES;
        ptearr[currSize].isValid = 0;
        startMem = sizeof(PageTable);
        endMem = totalBytesMalloced - sizeof(Stack);
        *reinterpret_cast<uint32_t*>(startBuffer+startMem) = endMem-startMem+1;
        *reinterpret_cast<uint32_t*>(startBuffer+endMem-4000) = endMem-startMem+1;
        ++currSize;
        cout << "PageTable created" << endl;
    }
    void insert(const pageTableEntry& pte)
    {
        if(currSize == MAX_PAGE_TABLE_ENTRIES)
        {
            cerr << "Page table full" << endl;
            exit(1);
        }
        ptearr[currSize] = pte;
        ptearr[lastFreeIdx].nextFreeIdx = currSize;
        lastFreeIdx = currSize;
        cout << "New page table entry at " << currSize << " index" << endl;
        ++currSize;
    }
    uint16_t findFreeBlockIdx(uint32_t bytes)
    {
        for(uint16_t currIdx = firstFreeIdx; currIdx != MAX_PAGE_TABLE_ENTRIES; currIdx = ptearr[currIdx].nextFreeIdx)
        {
            if((*reinterpret_cast<uint32_t*>(startBuffer+ptearr[currIdx].headOffset)) >= bytes && (!ptearr[currIdx].isValid))    // as higher 31 bits store the actual length
            {
                return currIdx;
                break;
            }
        }
        return MAX_PAGE_TABLE_ENTRIES;
    }
    uint16_t allocateList(uint32_t bytes)
    {
        uint16_t idx = findFreeBlockIdx(bytes+8);           // because we need to store header & tail of implicit list block
        if(idx == MAX_PAGE_TABLE_ENTRIES)
        {
            cout << "No free block exists!\n" << endl;
            return idx;
        }
        cout << "Free Block found at head offset " << ptearr[idx].headOffset << endl;
        uint32_t len = *reinterpret_cast<uint32_t*>(startBuffer+ptearr[idx].headOffset);
        if(len > bytes+8)
        {
            pageTableEntry newent;
            newent.headOffset = ptearr[idx].headOffset+bytes+8;
            newent.isValid = 0;
            newent.nextFreeIdx = MAX_PAGE_TABLE_ENTRIES;
            insert(newent);
            *reinterpret_cast<uint32_t*>(startBuffer+ptearr[idx].headOffset+len-4) = len-bytes-8;
            *reinterpret_cast<uint32_t*>(startBuffer+ptearr[idx].headOffset+bytes+8) = len-bytes-8;
            *reinterpret_cast<uint32_t*>(startBuffer+ptearr[idx].headOffset+bytes+4) = bytes+8;
            *reinterpret_cast<uint32_t*>(startBuffer+ptearr[idx].headOffset) = bytes+8;
        }
        ptearr[idx].isValid = 1;
        cout << "List allocated" << endl;
        return idx;
    }
    void assignValUtil(uint16_t idx, uint32_t listIndex, uint32_t val)
    {
        uint32_t offset = listIndex<<2;         // offset in bytes
        uint32_t len = *reinterpret_cast<uint32_t*>(startBuffer+ptearr[idx].headOffset);
        if(len-8 <= offset)
        {
            cerr << "Invalid offset" << endl;
            exit(1);
        }
        *reinterpret_cast<uint32_t*>(startBuffer+ptearr[idx].headOffset+offset) = val;
        cout << "Value assigned at memory offset: " << ptearr[idx].headOffset+offset << endl;
    }
    uint32_t getValUtil(uint16_t idx, uint32_t listIndex)
    {
        uint32_t offset = listIndex<<2;
        if(ptearr[idx].headOffset-8 <= offset)
        {
            cerr << "Invalid offset" << endl;
            exit(1);
        }
        return *reinterpret_cast<uint32_t*>(startBuffer+ptearr[idx].headOffset+offset);
    }
    uint16_t findPTidxByHeadOffset(uint32_t headoffset)     // req by coalesce
    {
        for(int i = 0; i < currSize; i++)
        {
            if(ptearr[i].headOffset == headoffset)
            return i;
        }
        return MAX_PAGE_TABLE_ENTRIES;
    }
    // void coalesce(uint16_t idx)
    // {
    //     // check for front
    //     uint32_t len = *reinterpret_cast<uint32_t*>(startBuffer+ptearr[idx].headOffset);
    //     uint16_t nextBlockIdx = findPTidxByHeadOffset(ptearr[idx].headOffset+len);
    //     if(nextBlockIdx == -1)      // if no more block exists next to this block
    //     return;
    //     ptearr[nextBlockIdx].isValid = 1;       // making it NOT FREE
    //     *reinterpret_cast<uint32_t*>(startBuffer+ptearr[idx].headOffset) = 
    // }
    void freeList(uint16_t idx)
    {
        if(ptearr[idx].isValid == 0)        // if it was already freed earlier
        return;
        ptearr[idx].isValid = 0;
        ptearr[idx].nextFreeIdx = MAX_PAGE_TABLE_ENTRIES;
        ptearr[lastFreeIdx].nextFreeIdx = idx;
        lastFreeIdx = idx;
    }
};



#endif