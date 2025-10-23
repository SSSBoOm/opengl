#version 330 core
out vec4 FragColor;

uniform float time;

void main()
{
  float pulse = 0.5 + 0.5 * sin(time * 2.0);
  vec3 baseColor = mix(vec3(1.0, 0.45, 0.1), vec3(0.95, 0.6, 0.25), pulse);
  FragColor = vec4(baseColor, 1.0);
}
