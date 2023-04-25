#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

void childbkc()
{
    for(int i = 0; i < 10; i++)
    {
        if(fork() == 0)
        {
            cout << "GrandChildren created" << endl;
            while(1);
        }
    }
    while(1);
}

int main()
{
    while(1)
    {
        sleep(10);
        for(int i = 0; i < 5; i++)
        {
            if(fork() == 0)
            {
                cout << "Children created" << endl;
                childbkc();
                exit(0);
            }
        }
    }
    return 0;
}