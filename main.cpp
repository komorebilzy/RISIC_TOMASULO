#include <iostream>
#include "Tomasulo.h"
int main() {
    freopen("array_test1.data","r",stdin);
    order_input();
    reset();
    while(1){
        issue();
        execute();
        writeBack();
        commit();
    }
}
