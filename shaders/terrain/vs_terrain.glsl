#version 430 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4x4 uMVP;
uniform mat4x4 uModelView;
uniform float uScaleFactor = 1.0f;

out VDataBlock {
  vec3 normal;
  vec3 FragPos;
} OUT;

void main() {
  vec3 pos = uScaleFactor * inPosition.xyz;
  gl_Position = uMVP * vec4(pos, 1.0f);

  OUT.normal =inNormal;
  OUT.FragPos=vec3(uModelView * vec4(inPosition.xyz, 1.0));

}
