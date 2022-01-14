#version 430 core

layout (location=0) out vec4 color;

in VDataBlock {
  vec2 colorParticles;
} IN;

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

//  vec3  col =  pal( IN.colorParticles.x/100.0f, vec3(0.8,0.5,0.4),vec3(0.2,0.4,0.2),vec3(2.0,1.0,1.),vec3(0.,0.25,0.25));

  //vec3  col = pal(IN.colorParticles.x/150.0f, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(1.0,1.0,1.0),vec3(0.3,0.20,0.20) );
  vec3  col = pal(IN.colorParticles.x/150.0f, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(2.0,1.0,0.0),vec3(0.5,0.20,0.25) );
  color = vec4(sqrt(clamp(col, 0., 1.)), 1);

  //  color = vec4(0.15, 0.55, 0.75, 1.0);
  //  color=  vec4(rand(t.x, t.y), rand(t.x, t.x), rand(t.x - t.y, t.x), 1.0);
}
