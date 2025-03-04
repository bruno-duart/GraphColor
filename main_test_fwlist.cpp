#include "fwlist.h"
#include <iostream>

int main() {
    ForwardListWrapper flist;
    flist.push_front(1, 10, 0);
    flist.push_back(2, 20, 0);
    flist.push_front(0, 5, 0);
    flist.print();
    flist.pop_front();
    flist.print();

    return 0;
}