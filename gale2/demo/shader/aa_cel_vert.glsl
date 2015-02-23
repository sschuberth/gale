// http://prideout.net/blog/?p=22

#version 330 compatibility

uniform vec3 DiffuseMaterial;

out vec3 EyespaceNormal;
out vec3 Diffuse;

void main()
{
    EyespaceNormal = gl_NormalMatrix * gl_Normal;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    Diffuse = DiffuseMaterial;
}
