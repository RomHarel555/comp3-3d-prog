#version 440

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec3 v_color;
layout(location = 1) out vec2 v_texCoord;

layout(std140, binding = 0) uniform buf {
    mat4 mvp;
} ubuf;

out gl_PerVertex { vec4 gl_Position; };

void main()
{
    v_color = color;
    v_texCoord = texCoord;
    gl_Position = ubuf.mvp * vec4(position, 1.0);
}
