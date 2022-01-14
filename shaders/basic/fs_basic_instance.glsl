#version 430 core

out vec4 fragColor;

in VDataBlock {
  vec3 normal;
  vec3 FragPos;
  vec3 nNeighbors;
} IN;

uniform vec4 uColor;// = vec4(1.0f);

void main() {

  vec3 lightPos = vec3(100.0, 1000.0, 600.0);
  vec3 lightColor = vec3(0.75, 0.75, 0.75);

  vec3 ambient = vec3(0.7, 0.7, 0.7);

  vec3 norm = normalize(IN.normal);
  vec3 lightDir = normalize(lightPos - IN.FragPos);

  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  vec3 result = (ambient + diffuse) * IN.nNeighbors;
  fragColor = vec4(result, 1.0);

}
