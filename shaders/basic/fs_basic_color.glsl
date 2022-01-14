#version 430 core

layout(location = 0) out vec4 fragColor;


in VDataBlock {
  vec4 color;
} IN;

uniform vec4 uColor = vec4(1.0f);

void main() {
  fragColor = IN.color;
}
