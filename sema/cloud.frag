#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in float ParticleAlpha;

uniform sampler2D u_CloudTexture; 
uniform sampler2D u_SceneDepth;   


uniform vec2 u_ScreenSize; 
uniform float u_Softness; 

const float NEAR = 0.1; 
const float FAR  = 100.0;


float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Перевод в NDC [ -1..1 ]
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{
    vec4 texColor = texture(u_CloudTexture, TexCoords);
    
   
    vec2 screenUV = gl_FragCoord.xy / u_ScreenSize;

    
    float depthRaw = texture(u_SceneDepth, screenUV).r;
    float sceneZ = LinearizeDepth(depthRaw);

   
    float partZ = LinearizeDepth(gl_FragCoord.z);

    float diff = sceneZ - partZ;

  
    float fade = clamp(diff / u_Softness, 0.0, 1.0);

    
    FragColor = texColor;
    FragColor.a *= ParticleAlpha * fade;
}