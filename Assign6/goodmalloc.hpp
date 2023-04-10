#ifndef __GOOD_MALLOC_HPP
#define __GOOD_MALLOC_HPP

#include <iostream>
#include <cstring>
using namespace std;


const int MAX_FRAME_CNT = 100000;

char* startBuffer;                           

struct stackEntry
{
    uint32_t headOffset;                // stores offset of header of list
    char* listName;                     // Name of list 
    uint8_t listNameLen;                // assuming name of list can fit in 8 bits 
};

class FrameStack
{
    uint32_t fptrs[MAX_FRAME_CNT];           // array implementation of frame pointer stack 
    uint32_t currIdx;                        // current index in fptrs[]
    public:
    FrameStack();
    void push(const uint32_t& sp);           
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

#endif