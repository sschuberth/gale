// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

#include <gale/math/biasscale.h>
#include <gale/math/random.h>

using namespace gale::math;

int __cdecl main()
{
    RandomEcuyerf r;

    // Testing BiasScale.

    BiasScalef bs1(r.random01(),r.random01());

    BiasScalef bs2(bs1);
    bs2.invert();
    bs1.concat(bs2);
    assert(bs1.isNOP());

    BiasScalef bs3=!bs1;
    assert((bs1&bs3).isNOP());

    // Testing ScaleBias.

    ScaleBiasf sb1(r.random01(),r.random01());

    ScaleBiasf sb2(sb1);
    sb2.invert();
    sb1.concat(sb2);
    assert(sb1.isNOP());

    ScaleBiasf sb3=!sb1;
    assert((sb1&sb3).isNOP());

#if !defined NDEBUG && !defined GALE_TINY_CODE
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}
