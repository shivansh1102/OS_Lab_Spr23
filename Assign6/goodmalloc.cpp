#include "goodmalloc.hpp"

char* startBuffer;
uint32_t startMem, endMem;
uint32_t totalBytesMalloced;

Stack *stack;
PageTable* ptable;
// const uint32_t MAX_FRAME_CNT = 10000;
// const uint32_t MAX_PAGE_TABLE_ENTRIES = 16384;   // 2^15
// const uint32_t MAX_STACK_SIZE = 10000;

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

uint16_t Stack::findPTidxByName(char* listName, uint8_t listNameLen)
{
    // cout << fstack.top() << endl;
    uint32_t currsp = sp;
    char* temp = startBuffer+sp;
    uint8_t lNLen;
    while(currsp < fstack.top())
    {
        lNLen = *(reinterpret_cast<uint8_t*>(temp));
        cout << lNLen << endl;
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

uint16_t Stack::freeLastListCurrStack()
{
    if(sp == fstack.top())
    return MAX_PAGE_TABLE_ENTRIES;
    stackEntry se = top();
    pop();
    return se.pageTableIdx;
}

PageTable::PageTable() : firstFreeIdx(0), lastFreeIdx(0), currSize(0)
{
    ptearr[currSize].headOffset = sizeof(PageTable);
    ptearr[currSize].nextFreeIdx = MAX_PAGE_TABLE_ENTRIES;
    ptearr[currSize].isValid = 0;
    startMem = sizeof(PageTable);
    endMem = totalBytesMalloced - sizeof(Stack)-MAX_STACK_SIZE;
    *reinterpret_cast<uint32_t*>(startBuffer+startMem) = endMem-startMem+1;
    *reinterpret_cast<uint32_t*>(startBuffer+endMem) = endMem-startMem+1;
    ++currSize;
    cout << "PageTable created" << endl;
}

void PageTable::insert(const pageTableEntry& pte)
{
    if(currSize == MAX_PAGE_TABLE_ENTRIES)
    {
        cerr << "Page table full" << endl;
        exit(1);
    }
    ptearr[currSize] = pte;
    ptearr[lastFreeIdx].nextFreeIdx = currSize;
    lastFreeIdx = currSize;
    cout << "New page table entry created at " << currSize << " index" << endl;
    ++currSize;
}

uint16_t PageTable::findFreeBlockIdx(uint32_t bytes)
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

uint16_t PageTable::allocateList(uint32_t bytes)
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

void PageTable::assignValUtil(uint16_t idx, uint32_t listIndex, uint32_t val)
{
    uint32_t offset = listIndex<<2;         // offset in bytes
    uint32_t len = *reinterpret_cast<uint32_t*>(startBuffer+ptearr[idx].headOffset);
    cout << len << " " << offset << endl;
    if(len-8 <= offset)
    {
        
        cerr << "Invalid offset" << endl;
        exit(1);
    }
    *reinterpret_cast<uint32_t*>(startBuffer+ptearr[idx].headOffset+offset+4) = val;
    cout << "Value assigned at memory offset: " << ptearr[idx].headOffset+offset << endl;
}

uint32_t PageTable::getValUtil(uint16_t idx, uint32_t listIndex)
{
    uint32_t offset = listIndex<<2;
    if(ptearr[idx].headOffset-8 <= offset)
    {
        cerr << "Invalid offset" << endl;
        exit(1);
    }
    return *reinterpret_cast<uint32_t*>(startBuffer+ptearr[idx].headOffset+offset+4);
}

uint16_t PageTable::findPTidxByHeadOffset(uint32_t headoffset)     // req by coalesce
{
    for(int i = 0; i < currSize; i++)
    {
        if(ptearr[i].headOffset == headoffset)
        return i;
    }
    return MAX_PAGE_TABLE_ENTRIES;
}

void PageTable::freeList(uint16_t idx)
{
    if(ptearr[idx].isValid == 0)        // if it was already freed earlier
    return;
    ptearr[idx].isValid = 0;
    ptearr[idx].nextFreeIdx = MAX_PAGE_TABLE_ENTRIES;
    ptearr[lastFreeIdx].nextFreeIdx = idx;
    lastFreeIdx = idx;
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
    uint16_t ptidx = stack->findPTidxByName(listName, listNameLen+(uint8_t)1);
    if(ptidx == MAX_PAGE_TABLE_ENTRIES)
    {
        cerr << "No such list with given name exists" << endl;
        exit(1);
    }
    ptable->assignValUtil(ptidx, offset, val);
}

uint32_t getVal(char* listName, uint8_t listNameLen, uint32_t offset)
{
    uint16_t ptidx = stack->findPTidxByName(listName, listNameLen+(uint8_t)1);
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

void newFuncBegin()
{
    stack->newFuncBeginUtil();
}

void FuncEnd()
{
    stack->FuncEndUtil();
}

