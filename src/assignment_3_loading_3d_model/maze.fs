#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D wallTexture;

void main()
{
  FragColor = texture(wallTexture, TexCoord);
}
