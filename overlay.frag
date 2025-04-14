#version 440

layout(location = 0) in vec3 v_color;
layout(location = 1) in float v_alpha;

layout(location = 0) out vec4 fragColor;

void main()
{
    // Base color with alpha
    vec4 color = vec4(v_color, v_alpha);
    
    // Add pulsing effect
    float time = float(gl_PrimitiveID) * 0.001; // Use primitive ID as a simple time value
    float pulse = 0.5 + 0.3 * sin(time + gl_FragCoord.x * 0.01 + gl_FragCoord.y * 0.01);
    
    // Add text effect
    vec2 center = vec2(0.5, 0.5);
    vec2 texCoord = gl_FragCoord.xy / vec2(800, 600); // Assuming window size
    float dist = length(texCoord - center);
    
    // Create "GAME OVER" text effect
    if (dist < 0.3) {
        color.rgb = vec3(1.0, 0.0, 0.0); // Red text
        color.a = pulse;
    } else {
        color.a *= 0.5 * pulse; // Semi-transparent background
    }
    
    fragColor = color;
} 