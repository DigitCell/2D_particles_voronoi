#version 430 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 aOffset;
layout(location = 3) in vec3 numberNeighbors;

uniform mat4x4 uMVP;
uniform mat4x4 uModelView;
uniform float uScaleFactor = 1.0f;

out VDataBlock {
  vec3 normal;
  vec3 FragPos;
  vec3 nNeighbors;
} OUT;

void main() {
  vec3 pos = uScaleFactor * (inPosition.xyz+aOffset.xyz);
  gl_Position = uMVP * vec4(pos, 1.0f);

  OUT.normal =inNormal;
  OUT.FragPos=vec3(uModelView * vec4(inPosition.xyz+aOffset.xyz, 1.0));
  OUT.nNeighbors=numberNeighbors;

}
