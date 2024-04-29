in vec2 TexCoord;
in vec2 LightmapCoord;
in vec4 VertexColor;
out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;
uniform sampler2D texture6;
uniform sampler2D texture7;

// Parameters to adjust the lightmap contrast and brightness
uniform float contrast = 1.75;  // Increase or decrease to tweak contrast
uniform float brightness = 0.8;  // Increase or decrease to tweak brightness
uniform float minLightLevel = 0.02;
#define LIGHTMAP_CONTRAST_TEST

void main() {
    vec4 albedoColor = texture(texture0, TexCoord);
    #ifdef LIGHTMAP
         vec4 lightmapColor = texture(texture1, LightmapCoord);

#ifdef LIGHTMAP_CONTRAST_TEST        
        // Adjust lightmap brightness and contrast
        lightmapColor.rgb /= brightness;  // Lowering the overall brightness
        lightmapColor.rgb = (lightmapColor.rgb - 0.5) * contrast + 0.5;  // Adjusting contrast

        // Ensure there is a minimum light level
        lightmapColor.rgb = max(lightmapColor.rgb, vec3(minLightLevel));
#endif
        
        FragColor = albedoColor * lightmapColor;      
    #else
        FragColor = albedoColor * VertexColor;
    #endif
}