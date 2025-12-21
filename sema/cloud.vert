#version 330 core
layout (location = 0) in vec3 aQuadPos;       
layout (location = 1) in vec2 aTexCoords;     
layout (location = 2) in vec3 aInstancePos;   
layout (location = 3) in float aInstanceScale;
layout (location = 4) in float aInstanceAlpha;
out vec2 TexCoords;
out float ParticleAlpha;

uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoords = aTexCoords;
    ParticleAlpha = aInstanceAlpha;

    vec3 CameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 CameraUp    = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 vertexPos = aInstancePos
                   + CameraRight * aQuadPos.x * aInstanceScale
                   + CameraUp * aQuadPos.y * aInstanceScale;

    gl_Position = projection * view * vec4(vertexPos, 1.0);
}