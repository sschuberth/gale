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

float const CUBE_AMPLITUDE=2.4f;

class TestWindow:public DefaultWindow
{
  public:

    TestWindow():DefaultWindow("test_slerp",800,600),m_pause(false),m_freeze(false) {
        // Move the camera back to be able to see objects at the origin.
        m_camera.setPosition(Vec3f(0,0,8));

        // Specify two random orientations.
        RandomEcuyerf rand;
        m_k0=rand.getQuaternion();
        m_k1=rand.getQuaternion();

        // Set some OpenGL states.
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        glPolygonMode(GL_FRONT,GL_LINE);

        // Try to get 25 FPS even when events need to be processed.
        setTimeout(1.0/25.0);
    }

    bool onIdle() {
        double elapsed;

        if (m_pause) {
            m_timer.reset();
            return false;
        }

        if (m_freeze) {
            m_timer.getElapsedSeconds(elapsed);
            if (elapsed<1.0) {
                return false;
            }
            m_freeze=false;
            m_timer.reset();
        }

        static double t=0.0,sign=1.0;

        m_timer.getElapsedSeconds(elapsed);
        double step=sign*elapsed/5;
        m_timer.reset();

        t+=step;
        if (t<0.0) {
            t=0.0;
            sign=-sign;
            m_freeze=true;
        }
        if (t>1.0) {
            t=1.0;
            sign=-sign;
            m_freeze=true;
        }

        m_k=m_k0.getSlerp(m_k1,t);
        float x=static_cast<float>(sin(convDegToRad(t*180.0-90.0))*CUBE_AMPLITUDE);
        m_p=Vec3f(x,0,0);

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
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        m_camera.apply();

        HMat4f m;

        glColor3f(1.0f,1.0f,1.0f);

        m=m_k.getMatrix();
        m.setPositionVector(m_p);
        glPushMatrix();
        glMultMatrixf(m);
        drawCube(1.0);
        glPopMatrix();

        glColor3f(0.5f,0.5f,0.5f);

        // First orientation (and position).
        m=m_k0.getMatrix();
        m.setPositionVector(Vec3f(-CUBE_AMPLITUDE,0,0));
        glPushMatrix();
        glMultMatrixf(m);
        drawCube(1.0);
        glPopMatrix();

        // Second orientation (and position).
        m=m_k1.getMatrix();
        m.setPositionVector(Vec3f(CUBE_AMPLITUDE,0,0));
        glPushMatrix();
        glMultMatrixf(m);
        drawCube(1.0);
        glPopMatrix();
    }

    void onKeyEvent(char key) {
        if (key=='p') {
            m_pause=!m_pause;
        } else {
            DefaultWindow::onKeyEvent(key);
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

    Quatf m_k,m_k0,m_k1;
    Vec3f m_p;
    Timer m_timer;
    bool m_pause,m_freeze;
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
