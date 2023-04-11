#ifndef __GOOD_MALLOC_HPP
#define __GOOD_MALLOC_HPP

#include <iostream>
#include <cstring>
using namespace std;


const int MAX_FRAME_CNT = 10000;
const int MAX_PAGE_TABLE_ENTRIES = 32768;

char* startBuffer;                           

struct stackEntry
{
    uint16_t pageTableIdx;              // stores index of page table entry corresponding to this
    char* listName;                     // Name of list 
    uint8_t listNameLen;                // assuming name of list can fit in 8 bits 
    stackEntry()
    {
        pageTableIdx = -1;
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
    void newFuncBegin();                    // Save the current stack pointer as frame pointer in frame stack
    void FuncEnd();                         // Pop frame pointer entry
    void push(const stackEntry &);          // Push stack entry 
    stackEntry top();                       // Returns top entry from stack
    void pop();                             // Pop stack entry
};

struct pageTableEntry
{
    uint32_t headOffset;                    // stores offset of head of list corresponding to this entry if isValid=1
    u_int nextFreeIdx : 15;                 // stores index of next entry in PageTable having isValid=0 
    u_int isValid : 1;                      // isValid = 0 => chunk is free
    pageTableEntry()
    {
        headOffset = -1;
        nextFreeIdx = -1;
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
        ptearr[currSize].nextFreeIdx = -1;
        ptearr[currSize].isValid = 0;
        ++currSize;
    }
    void insert(const pageTableEntry& pte)
    {
        ptearr[currSize] = pte;
        ptearr[lastFreeIdx].nextFreeIdx = currSize;
        lastFreeIdx = currSize;
        ++currSize;
    }
    uint16_t findFreeBlockIdx(uint8_t bytes)
    {
        uint16_t idx = -1;
        for(uint16_t currIdx = firstFreeIdx; currIdx <= lastFreeIdx; currIdx = ptearr[currIdx].nextFreeIdx)
        {
            if(*reinterpret_cast<uint32_t*>(ptearr[currIdx].headOffset) >= bytes + 8)
            {
                idx = currIdx;
                break;
            }
        }
        return idx;
    }
    
};



#endif