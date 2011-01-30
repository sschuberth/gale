// http://machinesdontcare.wordpress.com/2008/10/29/subsurface-scatter-shader/

#version 330 compatibility

// Variables for lighting properties.
uniform float MaterialThickness;
uniform vec3 ExtinctionCoefficient;
uniform vec4 LightColor;
uniform vec4 BaseColor;
uniform vec4 SpecColor;
uniform float SpecPower;
uniform float RimScalar;

// Input variables from the VS.
in vec3 worldNormal, eyeVec, lightVec, vertPos, lightPos;

float halfLambert(in vec3 vect1, in vec3 vect2)
{
    float product = dot(vect1,vect2);
    return product * 0.5 + 0.5;
}

float blinnPhongSpecular(in vec3 normalVec, in vec3 lightVec, in float specPower)
{
    vec3 halfAngle = normalize(normalVec + lightVec);
    return pow(clamp(0.0,1.0,dot(normalVec,halfAngle)),specPower);
}

// Main fake sub-surface scatter lighting function.
vec4 subScatterFS()
{
    float attenuation = 10.0 * (1.0 / distance(lightPos,vertPos));
    vec3 eVec = normalize(eyeVec);
    vec3 lVec = normalize(lightVec);
    vec3 wNorm = normalize(worldNormal);

    vec4 dotLN = vec4(halfLambert(lVec,wNorm) * attenuation);
    dotLN *= BaseColor;

    vec3 indirectLightComponent = vec3(MaterialThickness * max(0.0,dot(-wNorm,lVec)));
    indirectLightComponent += MaterialThickness * halfLambert(-eVec,lVec);
    indirectLightComponent *= attenuation;
    indirectLightComponent.r *= ExtinctionCoefficient.r;
    indirectLightComponent.g *= ExtinctionCoefficient.g;
    indirectLightComponent.b *= ExtinctionCoefficient.b;

    vec3 rim = vec3(1.0 - max(0.0,dot(wNorm,eVec)));
    rim *= rim;
    rim *= max(0.0,dot(wNorm,lVec)) * SpecColor.rgb;

    vec4 finalCol = dotLN + vec4(indirectLightComponent,1.0);
    finalCol.rgb += (rim * RimScalar * attenuation * finalCol.a);
    finalCol.rgb += vec3(blinnPhongSpecular(wNorm,lVec,SpecPower) * attenuation * SpecColor * finalCol.a * 0.05);
    finalCol.rgb *= LightColor.rgb;

    return finalCol;
}

void main()
{
    gl_FragColor = subScatterFS();
}
