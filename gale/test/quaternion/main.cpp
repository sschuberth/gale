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

    TestWindow():DefaultWindow("test_slerp",800,600),m_pause(false),m_cubic(false) {
        // Move the camera back to be able to see objects at the origin.
        m_camera.setPosition(Vec3f(0,0,20));

        // Use spherical-linear interpolation by default.
        m_interpolator=&Quatf::getSlerp;

        // Specify two random orientations.
        RandomEcuyerf rand;
        m_k0=rand.getQuaternion();
        m_k1=rand.getQuaternion();
        m_k2=rand.getQuaternion();
        m_k3=rand.getQuaternion();

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

        double elapsed;
        m_timer.getElapsedSeconds(elapsed);
        double step=elapsed/5.0;
        m_timer.reset();

        if (m_cubic) {
            m_k=m_k0.getScherp(m_k3,t/4.0,m_k1,m_k2);
        } else {
            if (t>=0.0 && t<1.0) {
                m_k=(m_k0.*m_interpolator)(m_k1,t);
            } else if (t>=1.0 && t<2.0) {
                m_k=(m_k1.*m_interpolator)(m_k2,t-1.0);
            } else if (t>=2.0 && t<3.0) {
                m_k=(m_k2.*m_interpolator)(m_k3,t-2.0);
            } else if (t>=3.0 && t<4.0) {
                m_k=(m_k3.*m_interpolator)(m_k0,t-3.0);
            }
        }

        if (t>=4.0) {
            t-=4.0;
        } else {
            t+=step;
        }

        float angle=static_cast<float>(2.0*M_PI/4.0*t+M_PI/4.0);
        m_p=Vec3f(cosf(angle)*RADIUS,sinf(angle)*RADIUS,0);

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

        glColor3f(0.5f,0.5f,0.5f);

        // First key frame.
        m=m_k0.getMatrix();
        m.setPositionVector(Vec3f(+CATHETUS,+CATHETUS,0));
        glPushMatrix();
        glMultMatrixf(m);
        drawCube(1.0);
        glPopMatrix();

        // Second key frame.
        m=m_k1.getMatrix();
        m.setPositionVector(Vec3f(-CATHETUS,+CATHETUS,0));
        glPushMatrix();
        glMultMatrixf(m);
        drawCube(1.0);
        glPopMatrix();

        // Third key frame.
        m=m_k2.getMatrix();
        m.setPositionVector(Vec3f(-CATHETUS,-CATHETUS,0));
        glPushMatrix();
        glMultMatrixf(m);
        drawCube(1.0);
        glPopMatrix();

        // Fourth key frame.
        m=m_k3.getMatrix();
        m.setPositionVector(Vec3f(+CATHETUS,-CATHETUS,0));
        glPushMatrix();
        glMultMatrixf(m);
        drawCube(1.0);
        glPopMatrix();

        glColor3f(1.0f,1.0f,1.0f);

        // Interpolated orientation and position.
        m=m_k.getMatrix();
        m.setPositionVector(m_p);
        glPushMatrix();
        glMultMatrixf(m);
        drawCube(1.0);
        glPopMatrix();
    }

    void onKeyEvent(char key) {
        switch (key) {
            case 'p': {
                m_pause=!m_pause;
                break;
            }
            case 'l': {
                m_cubic=false;
                m_interpolator=&Quatf::getLerp;
                break;
            }
            case 's': {
                m_cubic=false;
                m_interpolator=&Quatf::getSlerp;
                break;
            }
            case 'c': {
                m_cubic=true;
                break;
            }
            default: {
                DefaultWindow::onKeyEvent(key);
                break;
            }
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
                m_camera.rotate(m_camera.getModelview().getUpVector(),convDegToRad(-mouse_diff_x)*0.5);
                m_camera.rotate(m_camera.getModelview().getRightVector(),convDegToRad(-mouse_diff_y)*0.5);
            }

            if (event&ME_BUTTON_MIDDLE) {
                m_camera.strafe(static_cast<float>(-mouse_diff_x)/100);
                m_camera.elevate(static_cast<float>(mouse_diff_y)/100);
            }

            if (event&ME_BUTTON_RIGHT) {
                m_camera.zoom(convDegToRad(mouse_diff_y));
            }

            repaint();
        }

        mouse_prev_x=x;
        mouse_prev_y=y;
    }

  private:

    // Use a member function pointer to easily switch between the interpolation methods.
    typedef Quatf (Quatf::*QuatfInterpolator)(Quatf const&,double) const;
    QuatfInterpolator m_interpolator;

    Quatf m_k,m_k0,m_k1,m_k2,m_k3;
    Vec3f m_p;

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

    ShowWindow(window.getWindowHandle(),SW_SHOW);
    window.processEvents();

#ifndef NDEBUG
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}
