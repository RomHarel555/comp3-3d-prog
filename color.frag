#version 440

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform LightingInfo {
    vec3 lightPosition;
    vec3 viewPosition;
    vec3 lightColor;
    float ambientStrength;
    float specularStrength;
    float shininess;
} lightingInfo;

void main() {
    // Sample the texture
    vec4 texColor = texture(texSampler, fragTexCoord);
    
    // Basic lighting calculation
    vec3 ambient = lightingInfo.ambientStrength * lightingInfo.lightColor;
    
    // Use a default normal pointing up if you don't have normals
    vec3 normal = vec3(0.0, 1.0, 0.0);
    vec3 lightDir = normalize(lightingInfo.lightPosition - vec3(fragTexCoord, 0.0));
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightingInfo.lightColor;
    
    // Simplified specular component
    vec3 viewDir = normalize(lightingInfo.viewPosition - vec3(fragTexCoord, 0.0));
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), lightingInfo.shininess);
    vec3 specular = lightingInfo.specularStrength * spec * lightingInfo.lightColor;
    
    // Combine lighting with color and texture
    vec3 result = (ambient + diffuse + specular) * fragColor * texColor.rgb;
    
    // Output final color
    outColor = vec4(result, texColor.a);
}
