#include <gale/math/random.h>
#include <gale/system/cpuinfo.h>
#include <gale/system/timer.h>

#include <iostream>

// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#ifdef _DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

#define N 2000000

using namespace gale::math;
using namespace gale::system;
using namespace std;

int main(int argc,char *argv[]) {
    RandomEcuyerf r;

    Timer timer;

    unsigned int half0=0,half1=0;
    unsigned int even=0,odd=0;

    timer.start();
    int i;
    for (i=N;i>=0;--i) {
        unsigned int n=r.random();
    }
    double s;
    timer.stop(s);

    for (i=N;i>=0;--i) {
        unsigned int n=r.random();

        if (n<UINT_MAX/2) {
            ++half0;
        }
        else {
            ++half1;
        }

        if (n&1) {
            ++odd;
        }
        else {
            ++even;
        }
    }

    cout << "random() method statistics" << endl << "----------------------" << endl << endl;
    cout << "range: " << half0 << " / " << half1 << endl;
    cout << "parity: " << even << " / " << odd << endl;
    cout << unsigned int(N*(1.0/s)) << " random numbers per second" << endl << endl;

    half0=half1=0;
    even=odd=0;

    srand((unsigned)time(NULL));

    timer.start();
    for (i=N;i>=0;--i) {
        unsigned int r=rand();
    }
    timer.stop(s);

    for (i=N;i>=0;--i) {
        unsigned int r=rand();

        if (r<RAND_MAX/2) {
            ++half0;
        }
        else {
            ++half1;
        }

        if (r&1) {
            ++odd;
        }
        else {
            ++even;
        }
    }

    cout << "rand() statistics" << endl << "-----------------" << endl << endl;
    cout << "range: " << half0 << " / " << half1 << endl;
    cout << "parity: " << even << " / " << odd << endl;
    cout << unsigned int(N*(1.0/s)) << " random numbers per second" << endl << endl;

    system("pause");

#ifndef NDEBUG
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}
