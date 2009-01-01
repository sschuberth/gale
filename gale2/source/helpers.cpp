#include "gale/math/color.h"
#include "gale/model/mesh.h"
#include "gale/wrapgl/camera.h"

namespace gale {

namespace wrapgl {

void pushOrtho2D()
{
    // Save the current matrix mode.
    GLint matrix_mode;
    glGetIntegerv(GL_MATRIX_MODE,&matrix_mode);

    // Set the modelview to identity.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Set the projection matrix to map each raster position to a screen pixel.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    glOrtho(viewport[0],viewport[2],viewport[1],viewport[3],-1,1);

    // Restore the matrix mode.
    glMatrixMode(matrix_mode);
}

void popOrtho2D()
{
    // Save the current matrix mode.
    GLint matrix_mode;
    glGetIntegerv(GL_MATRIX_MODE,&matrix_mode);

    // Restore the projection and modelview matrices.
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // Restore the matrix mode.
    glMatrixMode(matrix_mode);
}

void drawLogo()
{
    Camera* camera_orig=Camera::getAppliedCamera();

    // Apply the required camera settings.
    Camera camera_logo;

    int size=math::min(camera_logo.getScreenSpace().width,camera_logo.getScreenSpace().height)/6;
    camera_logo.setScreenSpaceDimensions(size,size);

    camera_logo.setProjection(math::Mat4d::Factory::PerspectiveProjection(size,size,90*math::Constd::DEG_TO_RAD()));

    float distance=3.0f;
    math::Vec3f normal(distance,distance,distance);
    camera_logo.setPosition(normal);
    camera_logo.setLookTarget(math::Vec3f::ZERO());

    camera_logo.apply();

    // Render the logo.
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glPushAttrib(GL_POLYGON_BIT);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LINE_SMOOTH);

    glColor3fv(math::Col3f::WHITE());

    // Render the cubes.
    model::Mesh::Renderer renderer;
    model::Mesh* cube=model::Mesh::Factory::Hexahedron();

    renderer.compile(cube);

    glTranslatef(1,0,0);
    renderer.render();

    glTranslatef(-1,1,0);
    renderer.render();

    glTranslatef(0,-1,1);
    renderer.render();

    glTranslatef(0,0,-1);

    delete cube;

    // Render the frame.
    float scale_corner=1.9f,scale_length=1.5f;
    math::Vec3f r(scale_corner,0,0),t(0,scale_corner,0),l(0,0,scale_corner),n;

    glBegin(GL_LINE_LOOP);

    glVertex3fv(r);

    n=(~((t-r)^normal))*scale_length;
    glVertex3fv(n+r);
    glVertex3fv(n+t);

    glVertex3fv(t);

    n=(~((l-t)^normal))*scale_length;
    glVertex3fv(n+t);
    glVertex3fv(n+l);

    glVertex3fv(l);

    n=(~((r-l)^normal))*scale_length;
    glVertex3fv(n+l);
    glVertex3fv(n+r);

    glEnd();

    glPopAttrib();
    glPopAttrib();
    glPopAttrib();

    if (camera_orig) {
        camera_orig->apply();
    }
}

} // namespace wrapgl

} // namespace gale
