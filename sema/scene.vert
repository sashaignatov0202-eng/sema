#version 330 core
layout (location = 0) in vec2 aPos; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(aPos.x, 0.0, aPos.y, 1.0);
    gl_Position = projection * view * worldPos;
}