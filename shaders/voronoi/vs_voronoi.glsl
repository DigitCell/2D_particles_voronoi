#version 330 core

layout(location=0) in vec3 pos;     /*  Absolute coordinates  */
layout(location=1) in vec4 offset;  /*  0 to 1 */
uniform vec2 scale;
uniform vec2 worldSize;
uniform vec2 iResolution;
uniform float radius;

out VDataBlock {
  vec2 colorParticles;
  vec2 posParticles;
  vec2 radius;
  vec2 Resolution;
} OUT;


void main()
{
    //vec2 scaled = pos.xy*scale*offset.w;
    vec2 scaled = pos.xy*offset.w*scale;
  //  vec2 scaled = vec2(pos.x /worldSize.x,pos.y /worldSize.y);//* (offset.z);
    gl_Position = vec4(scaled.x +2.0*offset.x/worldSize.x-1.0, scaled.y +2.0*offset.y/worldSize.y-1.0, pos.z, 1.0);
    OUT.colorParticles.x=offset.z;
    OUT.posParticles=offset.xy;
    OUT.radius.x=radius;
    OUT.Resolution=iResolution;

}
