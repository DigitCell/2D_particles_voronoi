#version 460 core

//layout (location=0) out vec4 color;

out vec4 fragColor;


in VDataBlock {
  vec2 amountParticles;
} IN;

float rand(float a, float b)
{
    return fract(sin(a*12.9898 + b*78.233) * 43758.5453);
  //  float t = 12.9898*a + 78.233*b;
  //  return fract((123.0+t) * sin(t));
}

void main()
{

    float deltaColor=IN.amountParticles.x;
    fragColor= vec4(0.1, 0.1+deltaColor*0.05, 0.1, 1.0);
  //  color=  vec4(rand(t.x, t.y), rand(t.x, t.x), rand(t.x - t.y, t.x), 1.0);
}
