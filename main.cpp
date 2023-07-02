#include <iostream>
#include "Tomasulo.h"
int main() {
    freopen("superloop.data","r",stdin);
    freopen("my_reg","w",stdout);
    order_input();
    reset();
    int num=0;
    while(1){
        ++num;
        if(num==4){
            int a=0;
        }
        issue();
        execute();
        writeBack();
        commit();
    }
}
