#version 460 core

layout(location=0) in vec2 pos;     /*  Absolute coordinates  */
layout(location=1) in float amountParticles;

uniform vec2 worldSize;
uniform int divider;

out VDataBlock {
  vec2 amountParticles;
} OUT;

void main()
{

    int instId=gl_InstanceID;
    vec2 worldSize2=worldSize*1.05;

    vec2 mapCellSize=worldSize/divider;
    float cubeX=mapCellSize.x*(pos.x+1.0) /worldSize2.x;
    float cubeY=mapCellSize.y*(pos.y+1.0) /worldSize2.y;
    vec2 scaled = vec2(cubeX, cubeY);//* (offset.z);
    vec2 offset;

    offset.x=mapCellSize.x*float(instId%divider);
    offset.y=mapCellSize.y*float(instId/divider);
    gl_Position = vec4(scaled.x +2.0*offset.x/worldSize.x-1.0, scaled.y +2.0*offset.y/worldSize.y-1.0, 0.0, 1.0);

    OUT.amountParticles.x=amountParticles;
    //OUT.amountParticles=gl_InstanceID;

}
