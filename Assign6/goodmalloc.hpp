#ifndef __GOOD_MALLOC_HPP
#define __GOOD_MALLOC_HPP

#include <iostream>
#include <cstring>
#include <fstream>
using namespace std;

#define MAX_FRAME_CNT 10000
#define MAX_PAGE_TABLE_ENTRIES 16384   // 2^15
#define MAX_STACK_SIZE 10000

extern char* startBuffer;                      // to store malloced segment header        
extern uint32_t startMem, endMem;              // to store offset of starting and ending portion of mem
extern uint32_t totalBytesMalloced;


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
    ~FrameStack() {}
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
    ~Stack() {}
    void newFuncBeginUtil();                    // Save the current stack pointer as frame pointer in frame stack
    void FuncEndUtil();                         // Pop frame pointer entry
    void push(const stackEntry &);          // Push stack entry 
    stackEntry top();                       // Returns top entry from stack
    void pop();                             // Pop stack entry
    uint16_t findPTidxByName(char*, uint8_t);
    uint16_t freeLastListCurrStack();
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
    PageTable() ;

    void insert(const pageTableEntry&);
    uint16_t findFreeBlockIdx(uint32_t);
    uint16_t allocateList(uint32_t);
    void assignValUtil(uint16_t, uint32_t, uint32_t);
    uint32_t getValUtil(uint16_t, uint32_t);
    uint16_t findPTidxByHeadOffset(uint32_t);     // req by coalesce
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
    void freeList(uint16_t);
};

void createMem(uint32_t);
void createList(uint32_t, char*, uint8_t);
void assignVal(char* , uint8_t, uint32_t, uint32_t);
uint32_t getVal(char*, uint8_t, uint32_t);
void freeElem();
void freeElem(char*, uint32_t);
void newFuncBegin();
void FuncEnd();

#endif