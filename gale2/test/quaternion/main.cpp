#include <gale/math/random.h>
#include <gale/system/timer.h>
#include <gale/wrapgl/defaultwindow.h>
#include <gale/wrapgl/renderer.h>

// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#if !defined NDEBUG && !defined GALE_TINY_CODE
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

#ifndef GALE_TINY_CODE
    #include <stdio.h>
#else
    #define printf print_str
#endif

using namespace gale::math;
using namespace gale::model;
using namespace gale::system;
using namespace gale::wrapgl;

#define CATHETUS 5.0f

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow(_T("test_quaternion"),800,600)
    ,   m_pause(false)
    ,   m_cubic(true)
    {
        // Move the camera back to be able to see objects at the origin.
        m_camera.approach(-20);

        // Create a cube mesh and prepare it for rendering.
        Mesh* cube=Mesh::Factory::Hexahedron();
        m_cube_prep.compile(*cube);
        delete cube;

        // Use spherical-linear interpolation by default.
        m_interpolator=&slerp;

        int i;

        // Get some random orientations.
        RandomEcuyerf rand(0x44766788);
        for (i=0;i<4;++i) {
            m_kf[i]=~rand.randomQuat();
        }

        // Calculate the tangents for the key frame orientations.
        for (i=0;i<4;++i) {
            m_kt[i]=m_kf[i].tangent(m_kf[(i-1)&3],m_kf[(i+1)&3]);
        }

        // Set some OpenGL states.
        glEnable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

        // Initialize the animation variables.
        onIdle();
    }

    bool onIdle() {
        if (m_pause) {
            m_timer.reset();
            return false;
        }

        static double t=0.0;
        int i=static_cast<int>(roundToZero(t));

        if (m_cubic) {
            m_k=squad(m_kf[i],m_kf[(i+1)&3],t-i,m_kt[i],m_kt[(i+1)&3],m_interpolator);
        }
        else {
            m_k=(*m_interpolator)(m_kf[i],m_kf[(i+1)&3],t-i);
        }

        static float const RADIUS=sqrtf(2*CATHETUS*CATHETUS);

        double angle=2.0*M_PI*t/4.0+M_PI/4.0;
        float x=static_cast<float>(cos(angle)*RADIUS);
        float y=static_cast<float>(sin(angle)*RADIUS);
        m_p=Vec3f(x,y,0);

        double elapsed;
        m_timer.elapsed(elapsed);
        double step=elapsed/5.0;
        m_timer.reset();

        t+=step;
        while (t>4.0) {
            t-=4.0;
        }

        return true;
    }

    void onRender() {
        glClear(GL_COLOR_BUFFER_BIT);

        m_camera.makeCurrent();

        HMat4f m;

        // Draw the keyframe cubes.
        glColor3f(0.5f,0.5f,0.5f);

        for (int i=0;i<4;++i) {
            m_kf[i].getMatrix(m);
            m.setPositionVector(Vec3f(CATHETUS*(1.0f-((i+1)&2)),CATHETUS*(1.0f-(i&2)),0));
            glPushMatrix();
            glMultMatrixf(m);
            Renderer::draw(m_cube_prep);
            glPopMatrix();
        }

        // Draw the interpolated cube.
        glColor3f(1.0f,1.0f,1.0f);

        m_k.getMatrix(m);
        m.setPositionVector(m_p);
        glPushMatrix();
        glMultMatrixf(m);
        Renderer::draw(m_cube_prep);
        glPopMatrix();
    }

    void onKeyEvent(char key) {
        switch (key) {
            case 'n': {
                // This will result in changes in rotation velocity within
                // interpolation intervals.
                m_interpolator=&nlerp;
                break;
            }
            case 's': {
                // This will result in constant rotation velocity within
                // interpolation intervals.
                m_interpolator=&slerp;
                break;
            }
            case 'c': {
                // This will result in smooth transitions between interpolation
                // intervals.
                m_cubic=!m_cubic;
                break;
            }

            case 'p': {
                m_pause=!m_pause;
                return;
            }
            default: {
                DefaultWindow::onKeyEvent(key);
                return;
            }
        }

        printf("Now using");
        if (m_interpolator==&nlerp) {
            printf(" normalized");
        }
        else if (m_interpolator==&slerp) {
            printf(" spherical");
        }
        printf(" lerp. Cubic interpolation is turned");
        if (m_cubic) {
            puts(" on.");
        }
        else {
            puts(" off.");
        }
    }

  private:

    PreparedMesh m_cube_prep;

    // Use a member function pointer to easily switch between the interpolation methods.
    Quatf::Interpolator m_interpolator;

    // Orientation and position of the interpolated cube.
    Quatf m_k;
    Vec3f m_p;

    // Key frame orientations and their tangents.
    Quatf m_kf[4];
    Quatf m_kt[4];

    Timer m_timer;
    bool m_pause,m_cubic;
};

int __cdecl main()
{
    // Make sure the window is destroyed before dumping memory leaks.
    {
        TestWindow window;

        ShowWindow(window.windowHandle(),SW_SHOW);
        window.processEvents();
    }

#if !defined NDEBUG && !defined GALE_TINY_CODE
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}
