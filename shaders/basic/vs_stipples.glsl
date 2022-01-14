#version 430 core

layout(location=0) in vec2 pos;     /*  Absolute coordinates  */
layout(location=1) in vec4 offset;  /*  0 to 1 */

/*  Seperate radii to compensate for window aspect ratio  */
uniform vec2 radius;
uniform vec2 worldSize;

out VDataBlock {
  vec2 colorParticles;
} OUT;

void main()
{
    vec2 radius2=radius;
    //vec2 radius2=vec2(0.01,0.01);
    vec2 scaled = vec2(radius.x*offset.w*pos.x /worldSize.x, radius.x*offset.w*pos.y /worldSize.y);//* (offset.z);
    gl_Position = vec4(scaled.x +2.0*offset.x/worldSize.x-1.0, scaled.y +2.0*offset.y/worldSize.y-1.0, 0.0, 1.0);

    OUT.colorParticles.x=offset.z;
    OUT.colorParticles.y=offset.w;

}
