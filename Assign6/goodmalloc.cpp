#include "goodmalloc.hpp"

Stack *stack;
PageTable* ptable;


FrameStack::FrameStack() : currIdx(0) 
{
    cout << "FrameStack created" << endl;
}

void FrameStack::push(const uint32_t& sp)
{
    if(currIdx == MAX_FRAME_CNT)
    {
        cerr << "Frame limit exceeded" << endl;
        exit(1);
    }
    fptrs[currIdx++] = sp;
    cout << "New frame created: frame pointer: " << sp << endl;
}
void FrameStack::pop()
{
    if(currIdx == 0)
    {
        cerr << "Can't pop empty stack" << endl;
        exit(1);
    }
    --currIdx;
}
uint32_t FrameStack::top()
{
    if(currIdx == 0)
    {
        cerr << "Empty frame stack" << endl;
        exit(1);
    }
    return fptrs[currIdx-1];
}

Stack::Stack(uint32_t endOffset) : sp(endOffset-(uint32_t)sizeof(Stack)) 
{
    cout << "Stack created. stack pointer: " << sp << endl;
}

void Stack::newFuncBeginUtil()
{
    fstack.push(sp);
}
void Stack::FuncEndUtil()
{
    fstack.pop();
}
void Stack::push(const stackEntry &entry)
{
    cout << "Old stack pointer: " << sp << endl;
    sp -= 2;
    char* temp = startBuffer+sp;
    *(reinterpret_cast<uint16_t*>(temp)) = entry.pageTableIdx;
    sp -= entry.listNameLen;
    temp = startBuffer+sp;
    strncpy(temp, entry.listName, entry.listNameLen);
    sp -= 1;
    temp = startBuffer+sp;
    *(reinterpret_cast<uint8_t*>(temp)) = entry.listNameLen;
    cout << "New entry pushed into stack. New stack pointer: " << sp << endl;
}
stackEntry Stack::top()
{
    stackEntry entry;
    char* temp = startBuffer+sp;
    entry.listNameLen = *(reinterpret_cast<uint8_t*>(temp));
    temp += 1;
    entry.listName = new char[entry.listNameLen];
    strncpy(entry.listName, temp, entry.listNameLen);
    temp += entry.listNameLen;
    entry.pageTableIdx = *(reinterpret_cast<uint16_t*>(temp));
    return entry;
}
void Stack::pop()
{
    char* temp = startBuffer+sp;
    uint8_t listNameLen = *(reinterpret_cast<uint8_t*>(temp));
    sp += 1 + listNameLen + 2;
}

void createMem(uint32_t bytes)
{
    startBuffer = new char[bytes];
    totalBytesMalloced = bytes;
    stack = new(startBuffer+bytes-sizeof(Stack)) Stack(bytes);
    ptable = new(startBuffer) PageTable();
    cout << "Memory Created!" << endl;
}

void createList(uint32_t size, char* listName, uint8_t listNameLen)
{
    size <<= 2;             // converting into bytes
    uint16_t ptidx = ptable->allocateList(size);
    if(ptidx == MAX_PAGE_TABLE_ENTRIES)
    return;
    printf("%s %d\n", listName, listNameLen);
    stackEntry se;
    se.listName = new char[listNameLen+1];      // for'\0'
    strncpy(se.listName, listName, listNameLen);
    se.listName[listNameLen] = '\0';
    se.listNameLen = listNameLen+(uint8_t)1;
    se.pageTableIdx = ptidx;
    stack->push(se);
    cout << "List created successfully" << endl;
    delete se.listName;
}

void assignVal(char* listName, uint8_t listNameLen, uint32_t offset, uint32_t val)
{
    uint16_t ptidx = stack->findPTidxByName(listName, listNameLen);
    if(ptidx == MAX_PAGE_TABLE_ENTRIES)
    {
        cerr << "No such list with given name exists" << endl;
        exit(1);
    }
    ptable->assignValUtil(ptidx, offset, val);
}

uint32_t getVal(char* listName, uint8_t listNameLen, uint32_t offset)
{
    uint16_t ptidx = stack->findPTidxByName(listName, listNameLen);
    if(ptidx == MAX_PAGE_TABLE_ENTRIES)
    {
        cerr << "No such list with given name exists" << endl;
        exit(1);
    }
    return ptable->getValUtil(ptidx, offset);
}

void freeElem()
{
    uint16_t ptidx;
    while((ptidx = stack->freeLastListCurrStack()) != MAX_PAGE_TABLE_ENTRIES)
    ptable->freeList(ptidx);
}

void freeElem(char* listName, uint32_t listNameLen)
{
    uint16_t ptidx = stack->findPTidxByName(listName, listNameLen);
    ptable->freeList(ptidx);
}


void testFunc()
{

}

void newFuncBegin()
{
    stack->newFuncBeginUtil();
}

void FuncEnd()
{
    stack->FuncEndUtil();
}

int main()
{
    cout << sizeof(PageTable)  << " " << sizeof(Stack) << endl;
    const int SIZE = 1000*1000;
    createMem(SIZE);
    newFuncBegin();
    createList(100, "arr", 3);
    cout << "hi" << endl;
    for(int i = 0; i < 100; i++)
    assignVal("arr", 3, i, i+1);
    for(int i = 99; i >= 0; i--)
    cout << getVal("arr", 3, i) << endl;
    FuncEnd();
    return 0;
}