uniform ivec2 viewport;
uniform int sections;
uniform vec4 color0;
uniform vec4 color1;

void main()
{
    // Calculate the adjacent and opposite cathetus lengths.
    vec2 cath=vec2(gl_FragCoord.x,gl_FragCoord.y)-viewport*0.5;

    // Calculate the angle in range [0..360[.
    float angle=degrees(atan(cath.y,cath.x))+180.0;

    // Calculate the section we are in.
    float section=floor(angle/360.0*sections);

    gl_FragColor=mix(color0,color1,mod(section,2.0));
}
