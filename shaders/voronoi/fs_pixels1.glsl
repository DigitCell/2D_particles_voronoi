#version 330 core

in vec2 texCoord;

//uniform vec2 iMouse;
uniform vec2 iResolution;
uniform sampler2D compTexture;
uniform sampler2D compTexture0;
//uniform float iTime;

#define size iResolution.xy
#define pixel(a, p) texture(a, p/vec2(textureSize(a,0)))
#define texel(a, p) texelFetch(a, ivec2(p), 0)

#define N ivec2(1,1)
#define dt 1.5


const int tot_n = N.x*N.y;

float hash11(float p)
{
    p = fract(p * 15.1031);
    p *= p + 33.33;
    p *= p + p;
    return fract(p) - 0.5;
}

float hash12(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}


vec2 hash21(float p)
{
	vec3 p3 = fract(vec3(p) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.xx+p3.yz)*p3.zy);

}

vec2 hash22p(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx+33.33);
    return fract((p3.xx+p3.yz)*p3.zy);
}

const int k = 1664525;

ivec4 hash( ivec4 x )
{
    x = ((x>>8)^x.wxyz)*k;
    x = ((x>>8)^x.wxyz)*k;
    x = ((x>>8)^x.wxyz)*k;
    x = ((x>>8)^x.wxyz)*k;
    return ivec4(x);
}

ivec2 i2xy(int id)
{
    return ivec2(id%N.x, id/N.x);
}

int xy2i(ivec2 p)
{
    return p.x + p.y*N.x;
}

ivec2 cross_distribution(int i)
{
    return (1<<(i/4)) * ivec2( ((i&2)/2)^1, (i&2)/2 ) * ( 2*(i%2) - 1 );
}

out vec4 fragColor;

vec2 fragCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
#define PI 3.14159265359
#define TWOPI 6.28318530718

ivec4 get(ivec2 p)
{
    return floatBitsToInt(texel(compTexture, p));
}

ivec4 getb(int id)
{
    return floatBitsToInt(texel(compTexture, i2xy(id)));
}

vec2 hash2( vec2 p )
{
	// texture based white noise
//  return  texel(compTexture, p).xy;
  //return vec2(texture(compTexture, p).x,texture(compTexture, p).y);
	//return textureLod( iChannel0, (p+0.5)/256.0, 0.0 ).xy;
    float h=1.-texture(compTexture, p).x;
    float c =smoothstep(0., fwidth(h)*2., h -.1 )*h;
    c += (1.-smoothstep(0., fwidth(h)*3., h - .1))*c*.5;
    return vec2(clamp(c,0.,1.), h);



    // procedural white noise
	//return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

vec3 voronoi( in vec2 x )
{
    vec2 n = floor(x);
    vec2 f = fract(x);

    //----------------------------------
    // first pass: regular voronoi
    //----------------------------------
	vec2 mg, mr;

    float md = 8.0;
    for( int j=-1; j<=1; j++ )
    for( int i=-1; i<=1; i++ )
    {
        vec2 g = vec2(float(i),float(j));
		    vec2 o = hash2( x );

        vec2 r = g + o - f;
        float d = dot(r,r);

        if( d<md )
        {
            md = d;
            mr = r;
            mg = g;
        }
    }

    //----------------------------------
    // second pass: distance to borders
    //----------------------------------
    md = 8.0;
    for( int j=-2; j<=2; j++ )
    for( int i=-2; i<=2; i++ )
    {
        vec2 g = mg + vec2(float(i),float(j));
		    vec2 o = hash2( x );

        vec2 r = g + o - f;

        if( dot(mr-r,mr-r)>0.00001 )
        md = min( md, dot( 0.5*(mr+r), normalize(r-mr) ) );
    }

    return vec3( md, mr );
}


const float offset = 1.0 / 600.0;

void main()
 {

    vec2 uv = fragCoord/iResolution.xy;
    vec2 p=uv;


    fragColor = vec4(dd, 1.0);

}



/*
  vec2 q=fragCoord.xy/iResolution.xy;
  vec2 p = -1.0 + 2.0*q;

  // vec3 col = vec3(0.2,0.2,0.2);

 vec2 uv = fragCoord/iResolution.xy;

 ivec4 nb = get(ivec2(fragCoord.xy));

    // Time varying pixel color
  vec3 col =vec3(uv.xyx);//+ 0.5*cos(iTime+uv.xyx+vec3(0,1,1));

    // Output to screen
  // fragColor = vec4(col,1.0);
   //fragColor = texture(compTexture, texCoord);
   fragColor =nb;
   */
