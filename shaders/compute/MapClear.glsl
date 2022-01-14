#version 430
#extension GL_ARB_compute_variable_group_size : enable

#include "Common.glsl"

uniform int numPoints;
uniform float radius;
uniform vec2 worldSize;
uniform ivec2 mapSize;
uniform int mapDivider;
uniform vec2 mapCellSize;

void main() {

  int indx = int(gl_GlobalInvocationID.x);
  mapIndexSize[indx]=0;
  mapIndexSizeFloat[indx]=0.0;
}
