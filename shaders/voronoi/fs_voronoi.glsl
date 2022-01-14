#version 330 core

layout (location=0) out vec4 color;

in VDataBlock {
  vec2 colorParticles;
  vec2 posParticles;
  vec2 radius;
  vec2 Resolution;
} IN;

vec2 fragCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
#define PI 3.14159265359
#define TWOPI 6.28318530718


float radius=IN.radius.x;
vec2 iResolution=IN.Resolution;

//uniform sampler2D compTexture;
//uniform float iTime;

#define size iResolution.xy



float rand(float a, float b)
{
    return fract(sin(a*12.9898 + b*78.233) * 43758.5453);
  //  float t = 12.9898*a + 78.233*b;
  //  return fract((123.0+t) * sin(t));
}

//Iq palettes https://www.shadertoy.com/view/ll2GD3
vec3 pal( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d )
{
    return a + b*cos( 6.28318*(c*t+d) );
}

void main()
{

  vec2 uv =fragCoord;///iResolution.xy;
  //  color = vec4(color_, 1.0);
  vec3  col = pal(IN.colorParticles.x/170.0f, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(2.0,1.0,0.0),vec3(0.5,0.20,0.25) );
  // vec3  col =  pal( IN.colorParticles.x/100.0f, vec3(0.8,0.5,0.4),vec3(0.2,0.4,0.2),vec3(2.0,1.0,1.),vec3(0.,0.25,0.25));

//  vec3  col = pal(IN.colorParticles.x/150.0f, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(1.0,1.0,1.0),vec3(0.3,0.20,0.20) );
/*
   float df= length(uv-IN.posParticles);
   if(df>0)
   {
     float screenLengthSize=df*iResolution.x;
     if(df>radius)
     {
       col=vec3(0.9,0.9,0.9);
     }
     else
     {
       float dd=1.0-1.9*df/radius;
       dd=max(0.1,dd);
      // col *= 1.0 - smoothstep(length(uv - IN.posParticles), 0.0, 0.01)/radius;
       //col*=dd;
     }
   }


    //color = vec4(sqrt(clamp(col, 0., 1.)), 1);
    color = vec4(col*1.25f, 1);
*/
/*
    if( gl_FragCoord.z>0.79 && gl_FragCoord.z<0.99)
    {
      col=vec3(0.9,0.9,0.9);
    }
    */
  //  else
      //col=vec3(0.1,0.1,0.9);

    color = vec4(col*1.0f, 1);

}
