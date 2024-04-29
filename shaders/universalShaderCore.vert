layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aTexCoord;
layout (location = 2) in vec4 aLightmapCoord;
layout (location = 3) in vec4 aVertexColor;

out vec2 TexCoord;
out vec2 LightmapCoord;
out vec4 VertexColor;

uniform mat4 modelView;
uniform mat4 projection;

void main() {
    gl_Position = projection * modelView * vec4(aPos, 1.0);
    TexCoord = aTexCoord.xy;
    LightmapCoord = aLightmapCoord.xy;
    VertexColor = vec4(aVertexColor.r, aVertexColor.g, aVertexColor.b, max(aVertexColor.w * 1.5, 0.2));
}

