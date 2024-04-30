layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aTexCoord;

out vec2 TexCoord;

uniform mat4 modelView;
uniform mat4 projection;
uniform vec3 viewOrigin;

void main() {
    // Transform vertex position to clip space
    vec4 vertexPosition = modelView * vec4(aPos, 1.0);
    gl_Position = projection * vertexPosition;
    TexCoord = aTexCoord.xy;
}

