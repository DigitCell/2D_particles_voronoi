#version 430 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;


out VDataBlock {
  vec4 color;
} OUT;

uniform mat4x4 uMVP;
uniform float uScaleFactor = 1.0f;

void main() {
  vec3 pos = uScaleFactor * inPosition.xyz;
  gl_Position = uMVP * vec4(pos, 1.0f);
  OUT.color=inColor;
}
