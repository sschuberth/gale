#include <gale/math/random.h>
#include <gale/system/timer.h>
#include <gale/wrapgl/defaultwindow.h>

void drawCube(float edge) {
    edge/=2;

    for (int i=0;i<4;++i) {
        glPushMatrix();
        glRotatef(i*90.0f,0.0f,1.0f,0.0f);
        glTranslatef(0.0f,0.0f,edge);

        glBegin(GL_QUADS);
            glTexCoord2f(1.0f,0.0f);
            glVertex3f(+edge,+edge,0.0f);  // Upper right vertex.
            glTexCoord2f(0.0f,0.0f);
            glVertex3f(-edge,+edge,0.0f);  // Upper left vertex.
            glTexCoord2f(0.0f,1.0f);
            glVertex3f(-edge,-edge,0.0f);  // Lower left vertex.
            glTexCoord2f(1.0f,1.0f);
            glVertex3f(+edge,-edge,0.0f);  // Lower right vertex.
        glEnd();

        glPopMatrix();
    }

    glPushMatrix();
    glRotatef(90.0f,1.0f,0.0f,0.0f);
    glTranslatef(0.0f,0.0f,edge);

    glBegin(GL_QUADS);
        glTexCoord2f(1.0f,0.0f);
        glVertex3f(+edge,+edge,0.0f);  // Upper right vertex.
        glTexCoord2f(0.0f,0.0f);
        glVertex3f(-edge,+edge,0.0f);  // Upper left vertex.
        glTexCoord2f(0.0f,1.0f);
        glVertex3f(-edge,-edge,0.0f);  // Lower left vertex.
        glTexCoord2f(1.0f,1.0f);
        glVertex3f(+edge,-edge,0.0f);  // Lower right vertex.
    glEnd();

    glPopMatrix();

    glRotatef(-90.0f,1.0f,0.0f,0.0f);
    glTranslatef(0.0f,0.0f,edge);

    glBegin(GL_QUADS);
        glTexCoord2f(1.0f,0.0f);
        glVertex3f(+edge,+edge,0.0f);  // Upper right vertex.
        glTexCoord2f(0.0f,0.0f);
        glVertex3f(-edge,+edge,0.0f);  // Upper left vertex.
        glTexCoord2f(0.0f,1.0f);
        glVertex3f(-edge,-edge,0.0f);  // Lower left vertex.
        glTexCoord2f(1.0f,1.0f);
        glVertex3f(+edge,-edge,0.0f);  // Lower right vertex.
    glEnd();
}

using namespace gale::math;
using namespace gale::system;
using namespace gale::wrapgl;

float const CATHETUS=5.0f;
float const RADIUS=sqrtf(2*CATHETUS*CATHETUS);

class TestWindow:public DefaultWindow
{
  public:

    TestWindow()
    :   DefaultWindow("test_slerp",800,600)
    ,   m_pause(false)
    ,   m_cubic(true)
    {
        // Move the camera back to be able to see objects at the origin.
        m_camera.setPosition(Vec3f(0,0,20));

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
        glPolygonMode(GL_FRONT,GL_LINE);

        // Try to get 25 FPS even when events need to be processed.
        setTimeout(1.0/25.0);
    }

    bool onIdle() {
        if (m_pause) {
            m_timer.reset();
            return false;
        }

        static double t=0.0;
        int i=roundToZero(static_cast<float>(t));

        if (m_cubic) {
            m_k=squad(m_kf[i],m_kf[(i+1)&3],t-i,m_kt[i],m_kt[(i+1)&3],m_interpolator);
        }
        else {
            m_k=(*m_interpolator)(m_kf[i],m_kf[(i+1)&3],t-i);
        }

        double angle=2.0*M_PI*t/4.0+M_PI/4.0;
        float x=static_cast<float>(::cos(angle)*RADIUS);
        float y=static_cast<float>(::sin(angle)*RADIUS);
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

    void onTimeout() {
        // We will never be idle if the queue is filled with mouse and paint
        // events, so make sure we get an animation anyways.
        onIdle();
    }

    void onResize(int width,int height) {
        m_camera.setScreenSpaceDimensions(width,height);
        m_camera.setProjection(Mat4d::Factory::PerspectiveProjection(width,height));
    }

    void onPaint() {
        glClear(GL_COLOR_BUFFER_BIT);

        m_camera.apply();

        HMat4f m;

        // Draw the keyframe cubes.
        glColor3f(0.5f,0.5f,0.5f);

        for (int i=0;i<4;++i) {
            m_kf[i].toMatrix(m);
            m.setPositionVector(Vec3f(CATHETUS*(1.0f-((i+1)&2)),CATHETUS*(1.0f-(i&2)),0));
            glPushMatrix();
            glMultMatrixf(m);
            drawCube(1.0);
            glPopMatrix();
        }

        // Draw the interpolated cube.
        glColor3f(1.0f,1.0f,1.0f);

        m_k.toMatrix(m);
        m.setPositionVector(m_p);
        glPushMatrix();
        glMultMatrixf(m);
        drawCube(1.0);
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

        std::cout << "Now using";
        if (m_interpolator==&nlerp) {
            std::cout << " normalized";
        }
        else if (m_interpolator==&slerp) {
            std::cout << " spherical";
        }
        std::cout << " lerp. Cubic interpolation is turned";
        if (m_cubic) {
            std::cout << " on." << std::endl;
        }
        else {
            std::cout << " off." << std::endl;
        }
    }

    void onMouseEvent(int x,int y,int wheel,int event) {
        static int mouse_prev_x=0,mouse_prev_y=0;

        // This is positive if the mouse cursor was moved to the right.
        int mouse_diff_x=x-mouse_prev_x;

        // This is positive if the mouse cursor was moved downwards.
        int mouse_diff_y=y-mouse_prev_y;

        if (event!=ME_BUTTON_NONE) {
            if (event&ME_BUTTON_LEFT) {
                m_camera.rotate(m_camera.getModelview().getUpVector(),-mouse_diff_x*Constd::DEG_TO_RAD()*0.5);
                m_camera.rotate(m_camera.getModelview().getRightVector(),-mouse_diff_y*Constd::DEG_TO_RAD()*0.5);
            }

            if (event&ME_BUTTON_MIDDLE) {
                m_camera.strafe(static_cast<float>(-mouse_diff_x)/100);
                m_camera.elevate(static_cast<float>(mouse_diff_y)/100);
            }

            if (event&ME_BUTTON_RIGHT) {
                m_camera.zoom(mouse_diff_y*Constf::DEG_TO_RAD());
            }

            repaint();
        }

        mouse_prev_x=x;
        mouse_prev_y=y;
    }

  private:

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

// Enable memory leak detection, see:
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/vxcondetectingisolatingmemoryleaks.asp
#ifdef _DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

int main()
{
    TestWindow window;

    ShowWindow(window.windowHandle(),SW_SHOW);
    window.processEvents();

#ifndef NDEBUG
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}
