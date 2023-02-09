#include <bits/stdc++.h>

using namespace std;


int main(){

    int x=7;
    while(x--){
        int y=5;
        while(y--){
            if(fork()==0){
                int z=5;
                while(z--){
                    if(fork()==0){
                        cout<<"Child "<<getpid()<<endl;
                    }
                }
                while(1);
            }
        }
        sleep(20);
    }
}
