in vec2 TexCoord;
in vec2 LightmapCoord;
in vec4 VertexColor;
in vec3 viewDirection;
in vec3 lightDirection;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 EmissiveColor;

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
uniform float minLightLevel = 0.15;

#define LIGHTMAP_CONTRAST_TEST
#define FAKE_NORMALMAP

void main() {
    vec4 albedoColor = texture(texture0, TexCoord);

#ifdef FAKE_NORMALMAP
        vec2 uv = TexCoord;
        float mipLevel;

        {
            // Calculate the derivatives of the texture coordinates
            vec2 dx = dFdx(TexCoord);
            vec2 dy = dFdy(TexCoord);

            // Estimate the mipmap level based on the derivatives
            mipLevel = 0.5 * log2(max(dot(dx, dx), dot(dy, dy)));
        }

        // Clamp mipLevel to the range of existing mipmap levels
        mipLevel = clamp(mipLevel, 0.0, log2(float(textureSize(texture0, 0).x)));

        // Get the size of the texture at the estimated mipmap level
        ivec2 mipSize = textureSize(texture0, int(mipLevel));
        float width = 1.0 / mipSize.x;
        float height = 1.0 / mipSize.y;

        // Fetch the surrounding pixels in the lightmap
        float up = texture(texture0, uv + vec2(0.0, height)).r;
        float down = texture(texture0, uv - vec2(0.0, height)).r;
        float right = texture(texture0, uv + vec2(width, 0.0)).r;
        float left = texture(texture0, uv - vec2(width, 0.0)).r;

        // Calculate derivatives to approximate normals
        float dx = right - left;
        float dy = up - down;

        // Construct a normal from derivatives
        vec3 normal = normalize(vec3(dx, dy, 1.0));
#endif

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

#if defined(FAKE_NORMALMAP)
        //FragColor += specular;
        FragColor.xyz *= length(normal * normal * normal * normal);
#endif


#if !defined(LIGHTMAP) && defined(COLOR_BLEND)
     FragColor.a *= length(FragColor.xyz);
#endif

#ifndef EMISSIVE
#ifndef LIGHTMAP
    EmissiveColor = vec4(0, 0, 0, 0);
#else
    if(albedoColor.x > 0.95 && albedoColor.y > 0.95 && albedoColor.z > 0.95)
    {
        EmissiveColor = FragColor * 0.5;
    }
    else {
        EmissiveColor = vec4(0, 0, 0, 0);
    }
#endif
#else
    EmissiveColor = FragColor;
#endif

#ifndef LIGHTMAP
#ifdef EMISSIVE
    FragColor *= (VertexColor * 1.0);
#else
    FragColor *= (VertexColor * 2.0);
#endif
#endif
}