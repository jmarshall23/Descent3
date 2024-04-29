layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aTexCoord;
layout (location = 2) in vec4 aLightmapCoord;

out vec2 TexCoord;
out vec2 LightmapCoord;

uniform mat4 modelView;
uniform mat4 projection;

void main() {
    gl_Position = projection * modelView * vec4(aPos, 1.0);
    TexCoord = aTexCoord.xy;
    LightmapCoord = aLightmapCoord.xy;
}

