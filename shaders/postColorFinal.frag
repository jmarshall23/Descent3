in vec2 TexCoord;

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D texture0;
layout(binding = 1) uniform sampler2D texture1;

vec4 applyGaussianBlur(sampler2D tex, vec2 texCoord, vec2 resolution, bool horizontal) {
    float weight[9] = float[](0.05, 0.09, 0.12, 0.15, 0.16, 0.15, 0.12, 0.09, 0.05);

    // Initialize the result with the central sample.
    vec4 result = texture(tex, texCoord) * weight[0];

    // Apply Gaussian blur.
    if (horizontal) {
        for (int i = 1; i < 9; ++i) {
            result += texture(tex, texCoord + vec2(i * 1.0 / resolution.x, 0.0)) * weight[i];
            result += texture(tex, texCoord - vec2(i * 1.0 / resolution.x, 0.0)) * weight[i];
        }
    } else {
        for (int i = 1; i < 9; ++i) {
            result += texture(tex, texCoord + vec2(0.0, i * 1.0 / resolution.y)) * weight[i];
            result += texture(tex, texCoord - vec2(0.0, i * 1.0 / resolution.y)) * weight[i];
        }
    }
    return result;
}

vec4 blendBloom(vec4 original, vec4 bloom) {
    float luminance = dot(bloom.rgb, vec3(0.2126, 0.7152, 0.0722));
    return original + bloom * clamp(luminance, 0.0, 1.0);
}

void main() {
    ivec2 resolution = textureSize(texture1, 0);

    vec4 albedoColor = texture(texture0, TexCoord);
    vec4 emissiveColor = texture(texture1, TexCoord);

    vec4 horizontalBlur = applyGaussianBlur(texture1, TexCoord, vec2(resolution.x, resolution.y), true);
    vec4 verticalBlur = applyGaussianBlur(texture1, TexCoord, vec2(resolution.x, resolution.y), false);
    vec4 blurredTexture1 = (horizontalBlur + verticalBlur) * 0.5;

    // Threshold emissive colors
    vec4 bloom = blurredTexture1;

    FragColor = blendBloom(albedoColor, bloom * 8);
}
