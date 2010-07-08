// http://prideout.net/blog/?p=22

uniform vec3 DiffuseMaterial;

varying vec3 EyespaceNormal;
varying vec3 Diffuse;

void main()
{
    EyespaceNormal = gl_NormalMatrix * gl_Normal;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    Diffuse = DiffuseMaterial;
}
