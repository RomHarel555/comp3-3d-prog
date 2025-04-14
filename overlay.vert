#version 440

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 v_color;
layout(location = 1) out float v_alpha;

void main()
{
    v_color = color;
    v_alpha = 0.7; // Semi-transparent overlay
    gl_Position = position; // Use position directly since it's already in screen space
} 