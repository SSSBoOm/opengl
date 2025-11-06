#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

// optional instancing transform (for maze walls)
layout (location = 3) in mat4 instanceMatrix;

uniform bool useInstance; // tell shader which mode to use
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;

void main()
{
    mat4 worldMatrix = model;
    if (useInstance)
        worldMatrix = worldMatrix * instanceMatrix;

    gl_Position = projection * view * worldMatrix * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
