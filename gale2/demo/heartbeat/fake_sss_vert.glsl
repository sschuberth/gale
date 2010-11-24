// http://machinesdontcare.wordpress.com/2008/10/29/subsurface-scatter-shader/

#version 330 compatibility

// Set light-position.
uniform vec3 LightPosition;

// Output variables to the FS.
out vec3 worldNormal, eyeVec, lightVec, vertPos, lightPos;

void subScatterVS(in vec4 ecVert)
{
    lightVec = LightPosition - ecVert.xyz;
    eyeVec = -ecVert.xyz;
    vertPos = ecVert.xyz;
    lightPos = LightPosition;
}

void main()
{
    worldNormal = gl_NormalMatrix * gl_Normal;

    vec4 ecPos = gl_ModelViewProjectionMatrix * gl_Vertex;

    // Call function to set outputs for subscatter FS.
    subScatterVS(ecPos);

    // Transform vertex by modelview and projection matrices.
    gl_Position = ecPos;

    // Forward current texture coordinates after applying texture matrix.
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}
