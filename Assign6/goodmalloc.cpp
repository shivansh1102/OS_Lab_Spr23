#include "goodmalloc.hpp"

FrameStack::FrameStack() : currIdx(0) {}

void FrameStack::push(const uint32_t& sp)
{
    if(currIdx == MAX_FRAME_CNT)
    {
        cerr << "Frame limit exceeded" << endl;
        exit(1);
    }
    fptrs[currIdx++] = sp;
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
    return fptrs[currIdx];
}

Stack::Stack(uint32_t endOffset) : sp(endOffset-(uint32_t)sizeof(Stack)) {}

void Stack::newFuncBegin()
{
    fstack.push(sp);
}
void Stack::FuncEnd()
{
    fstack.pop();
}
void Stack::push(const stackEntry &entry)
{
    sp -= 4;
    char* temp = startBuffer+sp;
    *(reinterpret_cast<uint32_t*>(temp)) = entry.headOffset;
    sp -= entry.listNameLen;
    temp = startBuffer+sp;
    strcpy(temp, entry.listName);
    sp -= 1;
    temp = startBuffer+sp;
    *(reinterpret_cast<uint8_t*>(temp)) = entry.listNameLen;
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
    entry.headOffset = *(reinterpret_cast<uint32_t*>(temp));
    return entry;
}
void Stack::pop()
{
    char* temp = startBuffer+sp;
    uint8_t listNameLen = *(reinterpret_cast<uint8_t*>(temp));
    sp += 1 + listNameLen + 4;
}

int main()
{
    
    return 0;
}
