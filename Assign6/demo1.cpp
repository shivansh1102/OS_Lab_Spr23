#include <iostream>
#include <cstring>
#include <fstream>
#include "goodmalloc.hpp"
using namespace std;

void func1(int n,  ofstream& fileout)
{
    newFuncBegin();
    if(n == 0)
    {
        FuncEnd();
        return;
    }
    createList(10, "a", 1);
    for(int i = 0; i <  10; i++)
    assignVal("a", 1, i, i+n);
    fileout << "When n: " << n << endl;
    for(int i = 0; i < 10; i++)
    fileout << getVal("a", 1, i, 0) << " ";
    fileout << endl;
    func1(n-1, fileout);

    fileout << "When n: " << n << endl;
    for(int i = 0; i < 10; i++)
    fileout << getVal("a", 1, i, 0) << " ";
    fileout << endl;
    freeElem();
    FuncEnd();
}

int main()
{
    ofstream fileout("out.txt");
    cout << sizeof(PageTable)  << " " << sizeof(Stack) << endl;
    const int SIZE = 1000*1000;
    createMem(SIZE);
    newFuncBegin();
    func1(5, fileout);
    FuncEnd();
    return 0;
}