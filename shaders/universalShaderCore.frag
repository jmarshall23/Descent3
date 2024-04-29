in vec2 TexCoord;
in vec2 LightmapCoord;
out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;
uniform sampler2D texture6;
uniform sampler2D texture7;

void main() {
    vec4 albedoColor = texture(texture0, TexCoord);
    #ifdef LIGHTMAP
        vec4 lightmapColor = texture(texture1, LightmapCoord);
        FragColor = albedoColor * lightmapColor;        
    #else
        FragColor = albedoColor;
    #endif
}