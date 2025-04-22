#version 440

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPos; // This will be used for height-based coloring

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform LightingInfo {
    vec3 lightPosition;
    vec3 viewPosition;
    vec3 lightColor;
    float ambientStrength;
    float specularStrength;
    float shininess;
} lightingInfo;

vec3 getTerrainColor(float height) {
    // Define terrain colors based on height
    vec3 grassColor = vec3(0.2, 0.6, 0.1);   // Green for low areas
    vec3 mountainColor = vec3(0.6, 0.5, 0.4); // Brown/gray for higher areas
    vec3 peakColor = vec3(0.9, 0.9, 0.9);    // White for peaks
    
    // Normalize height to 0-1 range (assuming y is up)
    float normalizedHeight = clamp((height + 20.0) / 40.0, 0.0, 1.0);
    
    // Mix colors based on height
    vec3 terrainColor;
    if (normalizedHeight < 0.4) {
        terrainColor = mix(grassColor, mountainColor, normalizedHeight / 0.4);
    } else {
        terrainColor = mix(mountainColor, peakColor, (normalizedHeight - 0.4) / 0.6);
    }
    
    return terrainColor;
}

void main() {
    // Get terrain color based on height (y position)
    vec3 terrainColor = getTerrainColor(fragPos.y);
    
    // Mix with input color for flexibility
    vec3 baseColor = mix(terrainColor, fragColor, 0.3);
    
    // Enhanced ambient lighting to ensure visibility
    vec3 ambient = max(lightingInfo.ambientStrength * 1.5, 0.4) * lightingInfo.lightColor;
    
    // Use a default normal pointing up
    vec3 normal = vec3(0.0, 1.0, 0.0);
    vec3 lightDir = normalize(lightingInfo.lightPosition - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightingInfo.lightColor;
    
    // Simplified specular component
    vec3 viewDir = normalize(lightingInfo.viewPosition - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), lightingInfo.shininess);
    vec3 specular = lightingInfo.specularStrength * spec * lightingInfo.lightColor;
    
    // Combine lighting with color
    vec3 result = (ambient + diffuse + specular) * baseColor;
    
    // Enhance contrast
    result = pow(result, vec3(0.9));
    
    // Output final color
    outColor = vec4(result, 1.0);
} 