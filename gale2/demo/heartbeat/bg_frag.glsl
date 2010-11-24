#version 330 compatibility

uniform ivec2 viewport;
uniform int segments;
uniform vec4 color0;
uniform vec4 color1;

void main()
{
    // Calculate the adjacent and opposite cathetus lengths.
    vec2 cath=gl_FragCoord.xy-vec2(viewport)*0.5;

    // Calculate the angle in range [0,360[.
    float angle=degrees(atan(cath.y,cath.x))+180.0;

    // Calculate the segment we are in.
    float segment=floor(angle/360.0*float(segments));

    gl_FragColor=mix(color0,color1,mod(segment,2.0));
}
