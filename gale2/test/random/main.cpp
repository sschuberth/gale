// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

#include <gale/math/random.h>
#include <gale/system/cpuinfo.h>
#include <gale/system/timer.h>

#include <iostream>

#define N 2000000

using namespace gale::math;
using namespace gale::system;

int __cdecl main()
{
    RandomEcuyerf r;

    Timer timer;

    unsigned int half0=0,half1=0;
    unsigned int even=0,odd=0;

    timer.start();
    int i;
    for (i=N;i>=0;--i) {
        r.random();
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

#ifndef GALE_TINY_CODE
    std::cout << "random() method statistics" << std::endl << "----------------------" << std::endl << std::endl;
    std::cout << "range: " << half0 << " / " << half1 << std::endl;
    std::cout << "parity: " << even << " / " << odd << std::endl;
    std::cout << unsigned int(N*(1.0/s)) << " random numbers per second" << std::endl << std::endl;

    half0=half1=0;
    even=odd=0;

    srand((unsigned)time(NULL));

    timer.start();
    for (i=N;i>=0;--i) {
        rand();
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

    std::cout << "rand() statistics" << std::endl << "-----------------" << std::endl << std::endl;
    std::cout << "range: " << half0 << " / " << half1 << std::endl;
    std::cout << "parity: " << even << " / " << odd << std::endl;
    std::cout << unsigned int(N*(1.0/s)) << " random numbers per second" << std::endl << std::endl;
#endif

#if !defined NDEBUG && !defined GALE_TINY_CODE
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}
