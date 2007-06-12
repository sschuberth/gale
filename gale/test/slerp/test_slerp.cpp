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

    TestWindow():DefaultWindow("test_slerp",800,600),m_pause(false),m_freeze(false)
    {
        // Move the camera back to be able to see objects at the origin.
        m_camera.setPosition(Vec3f(0,0,8));

        // Specify two random orientations.
        RandomEcuyerf rand;
        m_k0=rand.getQuaternion();
        m_k1=rand.getQuaternion();

        // Set some OpenGL states.
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        glBlendFunc(GL_DST_ALPHA,GL_ONE_MINUS_DST_ALPHA);

        // Start timing.
        m_timer.start();
    }

    bool onIdle() {
        double elapsed;

        if (m_pause) {
            m_timer.start();
            return false;
        }

        if (m_freeze) {
            m_timer.getElapsedSeconds(elapsed);
            if (elapsed<1.0) {
                return false;
            }
            m_freeze=false;
            m_timer.start();
        }

        static double t=0.0,sign=1.0;

        m_timer.getElapsedSeconds(elapsed);
        double step=sign*elapsed/5;
        m_timer.start();

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

    void onResize(int width,int height) {
        m_camera.setScreenSpaceDimensions(width,height);
        m_camera.setProjection(Mat4d::Factory::PerspectiveProjection(width,height));
    }

    void onPaint() {
        glClearColor(0.0f,0.0f,0.0f,0.4f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        m_camera.apply();

        HMat4f m;

        m=m_k.getMatrix();
        m.setPositionVector(m_p);
        glPushMatrix();
        glMultMatrixf(m);
        drawCube(1.0);
        glPopMatrix();

        glEnable(GL_BLEND);

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

        glDisable(GL_BLEND);
    }

    void onKeyPress(char key) {
        if (key=='p') {
            m_pause=!m_pause;
        }
    }

  private:

    Quatf m_k,m_k0,m_k1;
    Vec3f m_p;
    Timer m_timer;
    bool m_pause,m_freeze;
};

//void TestWindow::onMouseMove(int x,int y,MouseEvent event) {
//    static int mouse_prev_x=0,mouse_prev_y=0;
//    int diff_x=x-mouse_prev_x,diff_y=y-mouse_prev_y;
//
//    if (event&MOUSE_BUTTON_LEFT) {
//        m_camera->rotate(m_camera->getUpDirection(),DEG2RAD(-diff_x)*0.5);
//        m_camera->rotate(m_camera->getNormalDirection(),DEG2RAD(-diff_y)*0.5);
//        repaint();
//    }
//    if (event&MOUSE_BUTTON_MIDDLE) {
//        m_camera->strafe(float(-diff_x)/144);
//        m_camera->fly(float(diff_y)/144);
//        repaint();
//    }
//    if (event&MOUSE_BUTTON_RIGHT) {
//        m_camera->zoom(float(diff_y)/20);
//        repaint();
//    }
//
//    mouse_prev_x=x;
//    mouse_prev_y=y;
//}

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
