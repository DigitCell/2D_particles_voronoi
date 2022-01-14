#version 330 core

in vec2 texCoord;

//uniform vec2 iMouse;
uniform vec2 iResolution;
uniform sampler2D compTexture;
//uniform float iTime;

#define size iResolution.xy
#define pixel(a, p) texture(a, p/vec2(textureSize(a,0)))
#define texel(a, p) texelFetch(a, ivec2(p), 0)

#define N ivec2(iResolution.x,iResolution.y)
#define dt 1.5


int tot_n = N.x*N.y;

ivec2 i2xy(int id)
{
    return ivec2(id%N.x, id/N.x);
}

int xy2i(ivec2 p)
{
    return p.x + p.y*N.x;
}

out vec4 fragColor;

vec2 fragCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
#define PI 3.14159265359
#define TWOPI 6.28318530718
#define texel(a, p) texelFetch(a, ivec2(p), 0)

ivec4 getClosest(ivec2 p)
{ return floatBitsToInt(texel(compTexture, p));}

vec4 getParticle(int id,out bool ghost)
{ vec4 p= texel(compTexture, i2xy(id));ghost=min(p.x,p.y)<0.;return vec4(abs(p.xy),p.zw); }

ivec4 get(ivec2 p)
{
    return floatBitsToInt(texel(compTexture, p));
}

ivec4 getb(int id)
{
    return floatBitsToInt(texel(compTexture, i2xy(id)));
}

void main()
 {

    vec2 uv = fragCoord/iResolution.xy;
    vec2 p=uv;


  fragColor += texture(compTexture, p);

}
