// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

#include <gale/global/dynamicarray.h>

using namespace gale::global;

int __cdecl main()
{
    {
        int i;

        DynamicArray<int> a;

        assert(!a.findSorted(42,i) && i==0);

        a.setSize(5);
        a=1,3,5,7,11;

        assert(a.findSorted(1,i) && i==0);
        assert(a.findSorted(3,i) && i==1);
        assert(a.findSorted(5,i) && i==2);
        assert(a.findSorted(7,i) && i==3);
        assert(a.findSorted(11,i) && i==4);

        assert(!a.findSorted(0,i) && i==0);
        assert(!a.findSorted(2,i) && i==1);
        assert(!a.findSorted(10,i) && i==4);
        assert(!a.findSorted(12,i) && i==5);

        a.insert(13);

        assert(a.findSorted(5,i) && i==2);
        assert(a.findSorted(7,i) && i==3);
        assert(a.findSorted(13,i) && i==5);

        assert(!a.findSorted(0,i) && i==0);
        assert(!a.findSorted(14,i) && i==6);
    }

#if !defined NDEBUG && !defined GALE_TINY_CODE
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}
