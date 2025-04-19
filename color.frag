#version 440

layout(location = 0) in vec3 v_color;
layout(location = 1) in vec2 v_texCoord;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 fragColor;

void main()
{
    vec4 texColor = texture(texSampler, v_texCoord);
    fragColor = vec4(v_color * texColor.rgb, 1.0);
}
