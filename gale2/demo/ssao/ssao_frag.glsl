// http://blenderartists.org/forum/showthread.php?t=184102

uniform sampler2D bgl_DepthTexture;
uniform sampler2D bgl_RenderedTexture;
uniform float bgl_RenderedTextureWidth;
uniform float bgl_RenderedTextureHeight;

#define PI    3.14159265

float width = bgl_RenderedTextureWidth; //texture width
float height = bgl_RenderedTextureHeight; //texture height

float near = 0.1; //Z-near
float far = 100.0; //Z-far

int samples = 6; //samples on the first ring (5-10)
int rings = 3; //ring count (2-6)

vec2 texCoord = gl_TexCoord[0].st;

vec2 rand(in vec2 coord) //generating random noise
{
    float noiseX = (fract(sin(dot(coord ,vec2(12.9898,78.233))) * 43758.5453));
    float noiseY = (fract(sin(dot(coord ,vec2(12.9898,78.233)*2.0)) * 43758.5453));
    return vec2(noiseX,noiseY);
}

float readDepth(in vec2 coord)
{
    return (2.0 * near) / (far + near - texture2D(bgl_DepthTexture, coord ).x * (far-near)); 
}

float compareDepths( in float depth1, in float depth2 )
{
    float aoCap = 1.0;
    float aoMultiplier = 100.0;
    float depthTolerance = 0.0000;
    float aorange = 6.0;// units in space the AO effect extends to (this gets divided by the camera far range
    float diff = sqrt(clamp(1.0-(depth1-depth2) / (aorange/(far-near)),0.0,1.0));
    float ao = min(aoCap,max(0.0,depth1-depth2-depthTolerance) * aoMultiplier) * diff;
    return ao;
}

void main(void)
{
    float depth = readDepth(texCoord);
    float d;

    float aspect = width/height;
    vec2 noise = rand(texCoord)*0.003;

    float w = (0.001/depth+noise.x);
    float h = (0.001/depth+noise.y);

    float pw;
    float ph;

    float ao;
    float s;

    for (int i = 0 ; i < rings; ++i)
    {
        for (int j = 0 ; j < samples; ++j)
        {
            float step = PI*2.0 / float(samples);
            pw = (cos(float(j)*step)*float(i));
            ph = (sin(float(j)*step)*float(i))*aspect;
            d = readDepth( vec2(texCoord.s+pw*w,texCoord.t+ph*h));
            ao += compareDepths(depth,d);
            s += 1.0;
        }
    }

    ao /= s;
    ao = 1.0-ao;

    vec3 lumcoeff = vec3(0.299,0.587,0.114);
    vec4 color = texture2D(bgl_RenderedTexture,gl_TexCoord[0]);
    float lum = dot(color.rgb, lumcoeff);
    vec3 luminance = vec3(lum, lum, lum);
    vec3 white = vec3(1.0,1.0,1.0);
    vec3 black = vec3(0.0,0.0,0.0);
    vec3 treshold = vec3(0.2,0.2,0.2);

    luminance = clamp(max(black,luminance-treshold)+max(black,luminance-treshold)+max(black,luminance-treshold),0.0,1.0);

    gl_FragColor = vec4(color*mix(vec3(ao,ao,ao),white,luminance),1.0);
}
