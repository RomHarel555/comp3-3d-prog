#include "RenderWindow.h"
#include <QVulkanFunctions>
#include <QFile>
#include "VulkanWindow.h"

// ENLARGED ground vertex data (10x10 plane instead of 5x5)
static float groundVertexData[] = {
    -10.0f,  0.0f, -10.0f,   0.3f, 0.3f, 0.3f,  // Bottom-left
    -10.0f,  0.0f,  10.0f,   0.3f, 0.3f, 0.3f,  // Top-left
     10.0f,  0.0f, -10.0f,   0.3f, 0.3f, 0.3f,  // Bottom-right

    -10.0f,  0.0f,  10.0f,   0.3f, 0.3f, 0.3f,  // Top-left
     10.0f,  0.0f,  10.0f,   0.3f, 0.3f, 0.3f,  // Top-right
     10.0f,  0.0f, -10.0f,   0.3f, 0.3f, 0.3f   // Bottom-right
};

// FIXED Player cube vertex data (MUCH brighter blue color for visibility)
static float playerVertexData[] = {
    // Front face
    -0.8f, -0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  // Bright blue
     0.8f, -0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  
    -0.8f,  0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f,  0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  
    -0.8f,  0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f, -0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  

    // Back face
    -0.8f, -0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f, -0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  
    -0.8f,  0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f,  0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  
    -0.8f,  0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f, -0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  

    // Left face
    -0.8f, -0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  
    -0.8f, -0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  
    -0.8f,  0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  
    -0.8f,  0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  
    -0.8f,  0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  
    -0.8f, -0.8f,  0.8f,   0.0f, 0.0f, 1.0f, 

    // Right face
     0.8f, -0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f, -0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f,  0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f,  0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f,  0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f, -0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  

    // Top face
    -0.8f,  0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f,  0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  
    -0.8f,  0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f,  0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  
    -0.8f,  0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f,  0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  

    // Bottom face
    -0.8f, -0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f, -0.8f, -0.8f,   0.0f, 0.0f, 1.0f,  
    -0.8f, -0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f, -0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  
    -0.8f, -0.8f,  0.8f,   0.0f, 0.0f, 1.0f,  
     0.8f, -0.8f, -0.8f,   0.0f, 0.0f, 1.0f    
};

// FIXED Collectible vertex data (VERY bright yellow color)
static float collectibleVertexData[] = {
    // Front face (pure bright yellow)
    -0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  // Pure yellow
     0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  
    -0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  
    -0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  

    // Back face
    -0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  
    -0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  
    -0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  

    // Left face
    -0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  
    -0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  
    -0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  
    -0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  
    -0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  
    -0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  

    // Right face
     0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  

    // Top face
    -0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  
    -0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  
    -0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  

    // Bottom face
    -0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f,  
    -0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  
    -0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,  
     0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f   
};

// NPC vertex data arrays with different colors for each NPC
// Red NPC vertex data (for NPC 1)
static float npcVertexData1[] = {
    // Front face (bright red color)
    -0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  // Pure red
     0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  
    -0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  
    -0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  

    // Back face
    -0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  
    -0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  
    -0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  

    // Left face
    -0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  
    -0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  
    -0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  
    -0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  
    -0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  
    -0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  

    // Right face
     0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  

    // Top face
    -0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  
    -0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  
    -0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  

    // Bottom face
    -0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f,  
    -0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  
    -0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,  
     0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f   
};

// Green NPC vertex data (for NPC 2)
static float npcVertexData2[] = {
    // Front face (bright green color)
    -0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,  // Pure green
     0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,  
    -0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,  
     0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,  
    -0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,  
     0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,  

    // Back face
    -0.7f, -0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  
     0.7f, -0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  
    -0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  
     0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  
    -0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  
     0.7f, -0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  

    // Left face
    -0.7f, -0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  
    -0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,  
    -0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  
    -0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,  
    -0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  
    -0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,  

    // Right face
     0.7f, -0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  
     0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,  
     0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  
     0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,  
     0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  
     0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,  

    // Top face
    -0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  
     0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  
    -0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,  
     0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,  
    -0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,  
     0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  

    // Bottom face
    -0.7f, -0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  
     0.7f, -0.7f, -0.7f,   0.0f, 1.0f, 0.0f,  
    -0.7f, -0.7f,  0.6f,   1.0f, 1.0f, 0.0f,  
     0.7f, -0.7f,  0.6f,   1.0f, 1.0f, 0.0f,  
    -0.7f, -0.7f,  0.6f,   1.0f, 1.0f, 0.0f,  
     0.7f, -0.7f, -0.6f,   1.0f, 1.0f, 0.0f   
};

// Blue NPC vertex data (for NPC 3)
static float npcVertexData3[] = {
    // Front face (bright blue color)
    -0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  // Bright blue
     0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  
    -0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  
    -0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  

    // Back face
    -0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  
    -0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  
    -0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  

    // Left face
    -0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  
    -0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  
    -0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  
    -0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  
    -0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  
    -0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  

    // Right face
     0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  

    // Top face
    -0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  
    -0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  
    -0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  

    // Bottom face
    -0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f,  
    -0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  
    -0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,  
     0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f   
};

//House vertex data - brown walls
static float houseWallsVertexData[] = {
    // Front wall (with door hole)
    // Left part of front wall
    -3.0f,  0.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-left
    -3.0f,  2.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Top-left
    -1.0f,  0.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-right
    -1.0f,  2.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Top-right
    -1.0f,  0.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-right
    -3.0f,  2.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Top-left

    // Right part of front wall
    1.0f,   0.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-left
    1.0f,   2.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Top-left
    3.0f,   0.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-right
    3.0f,   2.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Top-right
    3.0f,   0.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-right
    1.0f,   2.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Top-left

    // Top part of front wall
    -1.0f,  2.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-left
    -1.0f,  3.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Top-left
    1.0f,   2.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-right
    1.0f,   3.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Top-right
    1.0f,   2.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-right
    -1.0f,  3.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Top-left

    // Back wall
    -3.0f,  0.0f, -3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-left
    -3.0f,  3.0f, -3.0f,   0.6f, 0.4f, 0.2f,  // Top-left
    3.0f,   0.0f, -3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-right
    3.0f,   3.0f, -3.0f,   0.6f, 0.4f, 0.2f,  // Top-right
    3.0f,   0.0f, -3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-right
    -3.0f,  3.0f, -3.0f,   0.6f, 0.4f, 0.2f,  // Top-left

    // Left wall
    -3.0f,  0.0f, -3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-back
    -3.0f,  3.0f, -3.0f,   0.6f, 0.4f, 0.2f,  // Top-back
    -3.0f,  0.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-front
    -3.0f,  3.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Top-front
    -3.0f,  0.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-front
    -3.0f,  3.0f, -3.0f,   0.6f, 0.4f, 0.2f,  // Top-back

    // Right wall
    3.0f,   0.0f, -3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-back
    3.0f,   3.0f, -3.0f,   0.6f, 0.4f, 0.2f,  // Top-back
    3.0f,   0.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-front
    3.0f,   3.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Top-front
    3.0f,   0.0f,  3.0f,   0.6f, 0.4f, 0.2f,  // Bottom-front
    3.0f,   3.0f, -3.0f,   0.6f, 0.4f, 0.2f   // Top-back
};

// House door vertex data - dark brown (closed position)
static float houseDoorVertexData[] = {
    // Door (slightly inset)
    -1.0f,  0.0f,  2.9f,   0.4f, 0.2f, 0.1f,  // Bottom-left
    -1.0f,  2.0f,  2.9f,   0.4f, 0.2f, 0.1f,  // Top-left
    1.0f,   0.0f,  2.9f,   0.4f, 0.2f, 0.1f,  // Bottom-right
    1.0f,   2.0f,  2.9f,   0.4f, 0.2f, 0.1f,  // Top-right
    1.0f,   0.0f,  2.9f,   0.4f, 0.2f, 0.1f,  // Bottom-right
    -1.0f,  2.0f,  2.9f,   0.4f, 0.2f, 0.1f   // Top-left
};

// House door vertex data - dark brown (open position - rotated 90 degrees)
static float houseDoorOpenVertexData[] = {
    // Door (rotated to open position) - with a different color to make it obvious when open
    -1.0f,  0.0f,  2.9f,   0.9f, 0.5f, 0.2f,  // Bottom-left (hinge point) - brighter color
    -1.0f,  2.0f,  2.9f,   0.9f, 0.5f, 0.2f,  // Top-left (hinge point) - brighter color
    -1.0f,  0.0f,  0.9f,   0.9f, 0.5f, 0.2f,  // Bottom-right (new position) - brighter color
    -1.0f,  2.0f,  0.9f,   0.9f, 0.5f, 0.2f,  // Top-right (new position) - brighter color
    -1.0f,  0.0f,  0.9f,   0.9f, 0.5f, 0.2f,  // Bottom-right (new position) - brighter color
    -1.0f,  2.0f,  2.9f,   0.9f, 0.5f, 0.2f   // Top-left (hinge point) - brighter color
};

// House roof vertex data - red
static float houseRoofVertexData[] = {
    // Front triangle
    -3.5f,  3.0f,  3.5f,   0.8f, 0.2f, 0.2f,  // Bottom-left
    3.5f,   3.0f,  3.5f,   0.8f, 0.2f, 0.2f,  // Bottom-right
    0.0f,   5.0f,  0.0f,   0.8f, 0.2f, 0.2f,  // Top

    // Back triangle
    -3.5f,  3.0f, -3.5f,   0.8f, 0.2f, 0.2f,  // Bottom-left
    3.5f,   3.0f, -3.5f,   0.8f, 0.2f, 0.2f,  // Bottom-right
    0.0f,   5.0f,  0.0f,   0.8f, 0.2f, 0.2f,  // Top

    // Left triangle
    -3.5f,  3.0f, -3.5f,   0.8f, 0.2f, 0.2f,  // Bottom-back
    -3.5f,  3.0f,  3.5f,   0.8f, 0.2f, 0.2f,  // Bottom-front
    0.0f,   5.0f,  0.0f,   0.8f, 0.2f, 0.2f,  // Top

    // Right triangle
    3.5f,   3.0f, -3.5f,   0.8f, 0.2f, 0.2f,  // Bottom-back
    3.5f,   3.0f,  3.5f,   0.8f, 0.2f, 0.2f,  // Bottom-front
    0.0f,   5.0f,  0.0f,   0.8f, 0.2f, 0.2f   // Top
};

// Indoor walls vertex data - light beige
static float indoorWallsVertexData[] = {
    // Front wall (with door hole)
    // Left part of front wall
    -3.0f,  0.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-left
    -3.0f,  3.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Top-left
    -1.0f,  0.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-right
    -1.0f,  3.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Top-right
    -1.0f,  0.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-right
    -3.0f,  3.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Top-left

    // Right part of front wall
    1.0f,   0.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-left
    1.0f,   3.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Top-left
    3.0f,   0.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-right
    3.0f,   3.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Top-right
    3.0f,   0.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-right
    1.0f,   3.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Top-left

    // Top part of front wall
    -1.0f,  2.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-left
    -1.0f,  3.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Top-left
    1.0f,   2.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-right
    1.0f,   3.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Top-right
    1.0f,   2.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-right
    -1.0f,  3.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Top-left

    // Back wall
    -3.0f,  0.0f, -3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-left
    -3.0f,  3.0f, -3.0f,   0.9f, 0.8f, 0.7f,  // Top-left
    3.0f,   0.0f, -3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-right
    3.0f,   3.0f, -3.0f,   0.9f, 0.8f, 0.7f,  // Top-right
    3.0f,   0.0f, -3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-right
    -3.0f,  3.0f, -3.0f,   0.9f, 0.8f, 0.7f,  // Top-left

    // Left wall
    -3.0f,  0.0f, -3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-back
    -3.0f,  3.0f, -3.0f,   0.9f, 0.8f, 0.7f,  // Top-back
    -3.0f,  0.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-front
    -3.0f,  3.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Top-front
    -3.0f,  0.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-front
    -3.0f,  3.0f, -3.0f,   0.9f, 0.8f, 0.7f,  // Top-back

    // Right wall
    3.0f,   0.0f, -3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-back
    3.0f,   3.0f, -3.0f,   0.9f, 0.8f, 0.7f,  // Top-back
    3.0f,   0.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-front
    3.0f,   3.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Top-front
    3.0f,   0.0f,  3.0f,   0.9f, 0.8f, 0.7f,  // Bottom-front
    3.0f,   3.0f, -3.0f,   0.9f, 0.8f, 0.7f,  // Top-back
    
    // Ceiling
    -3.0f,  3.0f, -3.0f,   0.9f, 0.9f, 0.8f,  // Back-left
    3.0f,   3.0f, -3.0f,   0.9f, 0.9f, 0.8f,  // Back-right
    -3.0f,  3.0f,  3.0f,   0.9f, 0.9f, 0.8f,  // Front-left
    3.0f,   3.0f,  3.0f,   0.9f, 0.9f, 0.8f,  // Front-right
    -3.0f,  3.0f,  3.0f,   0.9f, 0.9f, 0.8f,  // Front-left
    3.0f,   3.0f, -3.0f,   0.9f, 0.9f, 0.8f,  // Back-right
    
    // Floor
    -3.0f,  0.0f, -3.0f,   0.7f, 0.6f, 0.5f,  // Back-left
    3.0f,   0.0f, -3.0f,   0.7f, 0.6f, 0.5f,  // Back-right
    -3.0f,  0.0f,  3.0f,   0.7f, 0.6f, 0.5f,  // Front-left
    3.0f,   0.0f,  3.0f,   0.7f, 0.6f, 0.5f,  // Front-right
    -3.0f,  0.0f,  3.0f,   0.7f, 0.6f, 0.5f,  // Front-left
    3.0f,   0.0f, -3.0f,   0.7f, 0.6f, 0.5f,  // Back-right
};

// Exit door vertex data (for inside)
static float exitDoorVertexData[] = {
    // Door (exit to outside)
    -1.0f,  0.0f,  3.0f,   0.4f, 0.8f, 0.4f,  // Bottom-left (green for exit)
    -1.0f,  2.0f,  3.0f,   0.4f, 0.8f, 0.4f,  // Top-left
    1.0f,   0.0f,  3.0f,   0.4f, 0.8f, 0.4f,  // Bottom-right
    1.0f,   2.0f,  3.0f,   0.4f, 0.8f, 0.4f,  // Top-right
    1.0f,   0.0f,  3.0f,   0.4f, 0.8f, 0.4f,  // Bottom-right
    -1.0f,  2.0f,  3.0f,   0.4f, 0.8f, 0.4f   // Top-left
};

//Utility variable and function for alignment:
static const int UNIFORM_DATA_SIZE = 16 * sizeof(float); //our MVP matrix contains 16 floats

// Forward declarations
static uint32_t getMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties& memProperties, 
                                 uint32_t typeBits, 
                                 VkMemoryPropertyFlags properties);

// Helper functions
static inline VkDeviceSize aligned(VkDeviceSize v, VkDeviceSize byteAlign)
{
    return (v + byteAlign - 1) & ~(byteAlign - 1);
}

/*** RenderWindow class ***/

RenderWindow::RenderWindow(QVulkanWindow *w, bool msaa)
    : mWindow(w),
    mPlayerPosition(0.0f, 0.0f, 0.0f),      // Player starts at center of platform
    mGameManager(nullptr),                   // Initialize to nullptr first
    mCollectedCount(0)                       // Initialize collected count
{
    if (msaa) {
        const QList<int> counts = w->supportedSampleCounts();
        qDebug() << "Supported sample counts:" << counts;
        for (int s = 16; s >= 4; s /= 2) {
            if (counts.contains(s)) {
                qDebug("Requesting sample count %d", s);
                w->setSampleCount(s);
                break;
            }
        }
    }
    
    // Initialize GameManager after member initialization
    mGameManager = new GameManager(this);
    
    // Initialize player position
    mPlayerPosition = QVector3D(0.0f, 0.0f, 0.0f);
    
    // Initialize collectibles
    initializeCollectibles();
    
    // Initialize NPCs
    initializeNPCs();
    
    // Debug initial setup
    qDebug() << "Initial player position:" << mPlayerPosition;
}

RenderWindow::~RenderWindow()
{
    delete mGameManager;
}

void RenderWindow::moveForward(float distance)
{
    // Store old position for debug output
    QVector3D oldPos = mPlayerPosition;
    
    // Move player along negative Z axis (forward in top-down view)
    // Multiply distance by 5 for faster movement
    mPlayerPosition.setZ(mPlayerPosition.z() - (distance * 5.0f));
    
    // Keep player on ground
    mPlayerPosition.setY(0.0f);
    
    qDebug() << "Moving player cube from" << oldPos << "to" << mPlayerPosition;
    
    // Request a redraw to show the player in its new position
    if (mWindow) {
        mWindow->requestUpdate();
    }
}

void RenderWindow::moveRight(float distance)
{
    // Store old position for debug output
    QVector3D oldPos = mPlayerPosition;
    
    // Move player along positive X axis (right in top-down view)
    // Multiply distance by 5 for faster movement
    mPlayerPosition.setX(mPlayerPosition.x() + (distance * 5.0f));
    
    // Keep player on ground
    mPlayerPosition.setY(0.0f);
    
    qDebug() << "Moving player cube from" << oldPos << "to" << mPlayerPosition;
    
    // Request a redraw to show the player in its new position
    if (mWindow) {
        mWindow->requestUpdate();
    }
}

void RenderWindow::moveCube(const QVector3D& movement)
{
    // Store old position for debug output
    QVector3D oldPos = mPlayerPosition;

    // Apply direct movement - make sure input controls are properly applied
    mPlayerPosition.setX(mPlayerPosition.x() + movement.x());
    mPlayerPosition.setZ(mPlayerPosition.z() + movement.z());
    
    // Keep within ground boundaries
    const float BOUNDARY = 9.5f; // Slightly smaller than the ground plane size
    mPlayerPosition.setX(qBound(-BOUNDARY, mPlayerPosition.x(), BOUNDARY));
    mPlayerPosition.setZ(qBound(-BOUNDARY, mPlayerPosition.z(), BOUNDARY));
    
    // Force Y to be 0 to keep player on ground
    mPlayerPosition.setY(0.0f);

    qDebug() << "PLAYER MOVED: From" << oldPos << "to" << mPlayerPosition 
             << "- Applied movement:" << movement;
             
    // Check for collectible collisions after movement
    checkCollectibleCollisions();

    // Request a redraw to update the scene
    if (mWindow) {
        mWindow->requestUpdate();
    }
}

void RenderWindow::rotate(float yawDelta, float pitchDelta)
{
    // Ignore rotation since we're using a fixed top-down view
    Q_UNUSED(yawDelta);
    Q_UNUSED(pitchDelta);
}

void RenderWindow::initResources()
{
    qDebug("\n ***************************** initResources ******************************************* \n");

    VkDevice logicalDevice = mWindow->device();
    mDeviceFunctions = mWindow->vulkanInstance()->deviceFunctions(logicalDevice);

    const int concurrentFrameCount = mWindow->concurrentFrameCount(); // 2 on Oles Machine
    const VkPhysicalDeviceLimits *pdevLimits = &mWindow->physicalDeviceProperties()->limits;
    const VkDeviceSize uniAlign = pdevLimits->minUniformBufferOffsetAlignment;
    qDebug("uniform buffer offset alignment is %u", (uint)uniAlign); //64 on Oles machine

    // Create uniform buffer for MVP matrices - allocate 8x the size to include indoor scene
    VkBufferCreateInfo bufInfo;
    memset(&bufInfo, 0, sizeof(bufInfo));
    bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    // Increased size to accommodate ground, player, collectibles, 3 separate NPCs, house, and indoor scene
    bufInfo.size = concurrentFrameCount * 8 * aligned(UNIFORM_DATA_SIZE, uniAlign);
    bufInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    // Use existing err variable
    VkResult err = mDeviceFunctions->vkCreateBuffer(logicalDevice, &bufInfo, nullptr, &mBuffer);
    if (err != VK_SUCCESS)
        qFatal("Failed to create uniform buffer: %d", err);

    VkMemoryRequirements memReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(logicalDevice, mBuffer, &memReq);

    VkMemoryAllocateInfo memAllocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        memReq.size,
        mWindow->hostVisibleMemoryIndex()
    };

    err = mDeviceFunctions->vkAllocateMemory(logicalDevice, &memAllocInfo, nullptr, &mBufferMemory);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate memory: %d", err);

    err = mDeviceFunctions->vkBindBufferMemory(logicalDevice, mBuffer, mBufferMemory, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind buffer memory: %d", err);

    // Setup uniform buffer descriptors
    quint8 *p;
    err = mDeviceFunctions->vkMapMemory(logicalDevice, mBufferMemory, 0, memReq.size, 0, reinterpret_cast<void **>(&p));
    if (err != VK_SUCCESS)
        qFatal("Failed to map memory: %d", err);

    QMatrix4x4 ident;
    memset(mUniformBufferInfo, 0, sizeof(mUniformBufferInfo));
    memset(mPlayerUniformBufferInfo, 0, sizeof(mPlayerUniformBufferInfo));
    memset(mCollectibleUniformBufferInfo, 0, sizeof(mCollectibleUniformBufferInfo));
    memset(mNPCUniformBufferInfo1, 0, sizeof(mNPCUniformBufferInfo1));
    memset(mNPCUniformBufferInfo2, 0, sizeof(mNPCUniformBufferInfo2));
    memset(mNPCUniformBufferInfo3, 0, sizeof(mNPCUniformBufferInfo3));
    memset(mHouseUniformBufferInfo, 0, sizeof(mHouseUniformBufferInfo));
    memset(mIndoorUniformBufferInfo, 0, sizeof(mIndoorUniformBufferInfo));
    
    const VkDeviceSize alignedUniformSize = aligned(UNIFORM_DATA_SIZE, uniAlign);
    
    for (int i = 0; i < concurrentFrameCount; ++i) {
        // Initialize the ground matrix (first matrix in the buffer)
        const VkDeviceSize groundOffset = i * 8 * alignedUniformSize; // *8 for ground, player, collectible, NPC1, NPC2, NPC3, house, indoor
        memcpy(p + groundOffset, ident.constData(), 16 * sizeof(float));
        mUniformBufferInfo[i].buffer = mBuffer;
        mUniformBufferInfo[i].offset = groundOffset;
        mUniformBufferInfo[i].range = UNIFORM_DATA_SIZE;
        
        // Initialize the player matrix space (second matrix in the buffer)
        const VkDeviceSize playerOffset = groundOffset + alignedUniformSize;
        memcpy(p + playerOffset, ident.constData(), 16 * sizeof(float));
        mPlayerUniformBufferInfo[i].buffer = mBuffer;
        mPlayerUniformBufferInfo[i].offset = playerOffset;
        mPlayerUniformBufferInfo[i].range = UNIFORM_DATA_SIZE;
        
        // Initialize the collectible matrix space (third matrix in the buffer)
        const VkDeviceSize collectibleOffset = playerOffset + alignedUniformSize;
        memcpy(p + collectibleOffset, ident.constData(), 16 * sizeof(float));
        mCollectibleUniformBufferInfo[i].buffer = mBuffer;
        mCollectibleUniformBufferInfo[i].offset = collectibleOffset;
        mCollectibleUniformBufferInfo[i].range = UNIFORM_DATA_SIZE;
        
        // Initialize the NPC1 matrix space (fourth matrix in the buffer)
        const VkDeviceSize npc1Offset = collectibleOffset + alignedUniformSize;
        memcpy(p + npc1Offset, ident.constData(), 16 * sizeof(float));
        mNPCUniformBufferInfo1[i].buffer = mBuffer;
        mNPCUniformBufferInfo1[i].offset = npc1Offset;
        mNPCUniformBufferInfo1[i].range = UNIFORM_DATA_SIZE;
        
        // Initialize the NPC2 matrix space (fifth matrix in the buffer)
        const VkDeviceSize npc2Offset = npc1Offset + alignedUniformSize;
        memcpy(p + npc2Offset, ident.constData(), 16 * sizeof(float));
        mNPCUniformBufferInfo2[i].buffer = mBuffer;
        mNPCUniformBufferInfo2[i].offset = npc2Offset;
        mNPCUniformBufferInfo2[i].range = UNIFORM_DATA_SIZE;
        
        // Initialize the NPC3 matrix space (sixth matrix in the buffer)
        const VkDeviceSize npc3Offset = npc2Offset + alignedUniformSize;
        memcpy(p + npc3Offset, ident.constData(), 16 * sizeof(float));
        mNPCUniformBufferInfo3[i].buffer = mBuffer;
        mNPCUniformBufferInfo3[i].offset = npc3Offset;
        mNPCUniformBufferInfo3[i].range = UNIFORM_DATA_SIZE;
        
        // Initialize the house matrix space (seventh matrix in the buffer)
        const VkDeviceSize houseOffset = npc3Offset + alignedUniformSize;
        memcpy(p + houseOffset, ident.constData(), 16 * sizeof(float));
        mHouseUniformBufferInfo[i].buffer = mBuffer;
        mHouseUniformBufferInfo[i].offset = houseOffset;
        mHouseUniformBufferInfo[i].range = UNIFORM_DATA_SIZE;
        
        // Initialize the indoor scene matrix space (eighth matrix in the buffer)
        const VkDeviceSize indoorOffset = houseOffset + alignedUniformSize;
        memcpy(p + indoorOffset, ident.constData(), 16 * sizeof(float));
        mIndoorUniformBufferInfo[i].buffer = mBuffer;
        mIndoorUniformBufferInfo[i].offset = indoorOffset;
        mIndoorUniformBufferInfo[i].range = UNIFORM_DATA_SIZE;
    }
    
    mDeviceFunctions->vkUnmapMemory(logicalDevice, mBufferMemory);

    /********************************* Vertex layout: *********************************/
    //The size of each vertex to be passed to the shader
    VkVertexInputBindingDescription vertexBindingDesc = {
        0, // binding - has to match that in VkVertexInputAttributeDescription and startNextFrame()s m_devFuncs->vkCmdBindVertexBuffers
        6 * sizeof(float), // stride account for X, Y, Z, R, G, B
        VK_VERTEX_INPUT_RATE_VERTEX
    };

    /********************************* Shader bindings: *********************************/
    //Descritpion of the attributes used in the shader
    VkVertexInputAttributeDescription vertexAttrDesc[] = {
        { // position
            0, // location has to correspond to the layout(location = x) in the shader
            0, // binding
            VK_FORMAT_R32G32B32_SFLOAT,
            0
        },
        { // color
            1, // location has to correspond to the layout(location = x) in the shader
            0, // binding
            VK_FORMAT_R32G32B32_SFLOAT,
            3 * sizeof(float) // offset to account for X, Y, Z
        }
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;
    vertexInputInfo.flags = 0;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDesc;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttrDesc;

    // Set up descriptor pool for all objects (ground, player, collectibles, and 3 separate NPCs)
    VkDescriptorPoolSize descPoolSizes = { 
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
        uint32_t(concurrentFrameCount * 10) // Ten objects: ground, player, collectible, NPC1, NPC2, NPC3, house walls, house door, house roof, spare
    };
    
    VkDescriptorPoolCreateInfo descPoolInfo;
    memset(&descPoolInfo, 0, sizeof(descPoolInfo));
    descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // Add this flag to allow freeing individual sets
    descPoolInfo.maxSets = concurrentFrameCount * 10; // Ten objects: ground, player, collectible, NPC1, NPC2, NPC3, house walls, house door, house roof, spare
    descPoolInfo.poolSizeCount = 1;
    descPoolInfo.pPoolSizes = &descPoolSizes;
    
    // Destroy old pool if it exists
    if (mDescriptorPool != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyDescriptorPool(mWindow->device(), mDescriptorPool, nullptr);
        mDescriptorPool = VK_NULL_HANDLE;
    }
    
    // Use existing err variable without redeclaring it
    err = mDeviceFunctions->vkCreateDescriptorPool(logicalDevice, &descPoolInfo, nullptr, &mDescriptorPool);
    if (err != VK_SUCCESS)
        qFatal("Failed to create descriptor pool: %d", err);

    /********************************* Uniform (projection matrix) bindings: *********************************/
    VkDescriptorSetLayoutBinding layoutBinding = {
        0, // binding
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        1,
        VK_SHADER_STAGE_VERTEX_BIT,
        nullptr
    };
    VkDescriptorSetLayoutCreateInfo descLayoutInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        nullptr,
        0,
        1,
        &layoutBinding
    };
    err = mDeviceFunctions->vkCreateDescriptorSetLayout(logicalDevice, &descLayoutInfo, nullptr, &mDescriptorSetLayout);
    if (err != VK_SUCCESS)
        qFatal("Failed to create descriptor set layout: %d", err);

    // Create separate descriptor sets for all objects
    for (int i = 0; i < concurrentFrameCount; ++i) {
        // Ground descriptor set
        VkDescriptorSetAllocateInfo descSetAllocInfo = {
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            nullptr,
            mDescriptorPool,
            1,
            &mDescriptorSetLayout
        };
        err = mDeviceFunctions->vkAllocateDescriptorSets(logicalDevice, &descSetAllocInfo, &mDescriptorSet[i]);
        if (err != VK_SUCCESS)
            qFatal("Failed to allocate ground descriptor set: %d", err);

        VkWriteDescriptorSet descWrite;
        memset(&descWrite, 0, sizeof(descWrite));
        descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descWrite.dstSet = mDescriptorSet[i];
        descWrite.descriptorCount = 1;
        descWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descWrite.pBufferInfo = &mUniformBufferInfo[i];
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 1, &descWrite, 0, nullptr);
        
        // Player descriptor set 
        err = mDeviceFunctions->vkAllocateDescriptorSets(logicalDevice, &descSetAllocInfo, &mPlayerDescriptorSet[i]);
        if (err != VK_SUCCESS)
            qFatal("Failed to allocate player descriptor set: %d", err);
            
        // Update player descriptor set
        descWrite.dstSet = mPlayerDescriptorSet[i];
        descWrite.pBufferInfo = &mPlayerUniformBufferInfo[i];
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 1, &descWrite, 0, nullptr);
        
        // Collectible descriptor set 
        err = mDeviceFunctions->vkAllocateDescriptorSets(logicalDevice, &descSetAllocInfo, &mCollectibleDescriptorSet[i]);
        if (err != VK_SUCCESS)
            qFatal("Failed to allocate collectible descriptor set: %d", err);
            
        // Update collectible descriptor set
        descWrite.dstSet = mCollectibleDescriptorSet[i];
        descWrite.pBufferInfo = &mCollectibleUniformBufferInfo[i];
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 1, &descWrite, 0, nullptr);
        
        // NPC 1 (Red) descriptor set 
        err = mDeviceFunctions->vkAllocateDescriptorSets(logicalDevice, &descSetAllocInfo, &mNPCDescriptorSet1[i]);
        if (err != VK_SUCCESS)
            qFatal("Failed to allocate NPC1 descriptor set: %d", err);
            
        // Update NPC1 descriptor set
        descWrite.dstSet = mNPCDescriptorSet1[i];
        descWrite.pBufferInfo = &mNPCUniformBufferInfo1[i];
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 1, &descWrite, 0, nullptr);
        
        // NPC 2 (Green) descriptor set 
        err = mDeviceFunctions->vkAllocateDescriptorSets(logicalDevice, &descSetAllocInfo, &mNPCDescriptorSet2[i]);
        if (err != VK_SUCCESS)
            qFatal("Failed to allocate NPC2 descriptor set: %d", err);
            
        // Update NPC2 descriptor set
        descWrite.dstSet = mNPCDescriptorSet2[i];
        descWrite.pBufferInfo = &mNPCUniformBufferInfo2[i];
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 1, &descWrite, 0, nullptr);
        
        // NPC 3 (Blue) descriptor set 
        err = mDeviceFunctions->vkAllocateDescriptorSets(logicalDevice, &descSetAllocInfo, &mNPCDescriptorSet3[i]);
        if (err != VK_SUCCESS)
            qFatal("Failed to allocate NPC3 descriptor set: %d", err);
            
        // Update NPC3 descriptor set
        descWrite.dstSet = mNPCDescriptorSet3[i];
        descWrite.pBufferInfo = &mNPCUniformBufferInfo3[i];
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 1, &descWrite, 0, nullptr);
    }

    // Pipeline cache
    VkPipelineCacheCreateInfo pipelineCacheInfo;
    memset(&pipelineCacheInfo, 0, sizeof(pipelineCacheInfo));
    pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    err = mDeviceFunctions->vkCreatePipelineCache(logicalDevice, &pipelineCacheInfo, nullptr, &mPipelineCache);
    if (err != VK_SUCCESS)
        qFatal("Failed to create pipeline cache: %d", err);

    // Pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    memset(&pipelineLayoutInfo, 0, sizeof(pipelineLayoutInfo));
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout;
    err = mDeviceFunctions->vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout);
    if (err != VK_SUCCESS)
        qFatal("Failed to create pipeline layout: %d", err);

    /********************************* Create shaders *********************************/
    //Creates our actuall shader modules
    VkShaderModule vertShaderModule = createShader(QStringLiteral(":/color_vert.spv"));
    VkShaderModule fragShaderModule = createShader(QStringLiteral(":/color_frag.spv"));

    // Graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo;
    memset(&pipelineInfo, 0, sizeof(pipelineInfo));
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    VkPipelineShaderStageCreateInfo shaderStages[2] = {
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            VK_SHADER_STAGE_VERTEX_BIT,
            vertShaderModule,
            "main",
            nullptr
        },
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            fragShaderModule,
            "main",
            nullptr
        }
    };

    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;

    VkPipelineInputAssemblyStateCreateInfo ia;
    memset(&ia, 0, sizeof(ia));
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInfo.pInputAssemblyState = &ia;

    // The viewport and scissor will be set dynamically via vkCmdSetViewport/Scissor.
    // This way the pipeline does not need to be touched when resizing the window.
    VkPipelineViewportStateCreateInfo vp;
    memset(&vp, 0, sizeof(vp));
    vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.viewportCount = 1;
    vp.scissorCount = 1;
    pipelineInfo.pViewportState = &vp;

    VkPipelineRasterizationStateCreateInfo rs;
    memset(&rs, 0, sizeof(rs));
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.polygonMode = VK_POLYGON_MODE_FILL;
    rs.cullMode = VK_CULL_MODE_NONE; // we want the back face as well
    rs.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rs.lineWidth = 1.0f;
    pipelineInfo.pRasterizationState = &rs;

    VkPipelineMultisampleStateCreateInfo ms;
    memset(&ms, 0, sizeof(ms));
    ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    // Enable multisampling.
    ms.rasterizationSamples = mWindow->sampleCountFlagBits();
    pipelineInfo.pMultisampleState = &ms;

    // Fix depth settings to avoid invisible objects
    VkPipelineDepthStencilStateCreateInfo ds;
    memset(&ds, 0, sizeof(ds));
    ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.depthTestEnable = VK_TRUE;
    ds.depthWriteEnable = VK_TRUE;
    ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;     // Ensures correct rendering order
    pipelineInfo.pDepthStencilState = &ds;

    VkPipelineColorBlendStateCreateInfo cb;
    memset(&cb, 0, sizeof(cb));
    cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    // no blend, write out all of rgba
    VkPipelineColorBlendAttachmentState att;
    memset(&att, 0, sizeof(att));
    att.colorWriteMask = 0xF;
    cb.attachmentCount = 1;
    cb.pAttachments = &att;
    pipelineInfo.pColorBlendState = &cb;

    VkDynamicState dynEnable[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dyn;
    memset(&dyn, 0, sizeof(dyn));
    dyn.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dyn.dynamicStateCount = sizeof(dynEnable) / sizeof(VkDynamicState);
    dyn.pDynamicStates = dynEnable;
    pipelineInfo.pDynamicState = &dyn;

    pipelineInfo.layout = mPipelineLayout;
    pipelineInfo.renderPass = mWindow->defaultRenderPass();

    err = mDeviceFunctions->vkCreateGraphicsPipelines(logicalDevice, mPipelineCache, 1, &pipelineInfo, nullptr, &mPipeline);
    if (err != VK_SUCCESS)
        qFatal("Failed to create graphics pipeline: %d", err);

    if (vertShaderModule)
        mDeviceFunctions->vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);
    if (fragShaderModule)
        mDeviceFunctions->vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);

    // Create and set up ground buffer
    VkBufferCreateInfo groundBufferInfo = {};
    groundBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    groundBufferInfo.size = sizeof(groundVertexData);
    groundBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    err = mDeviceFunctions->vkCreateBuffer(mWindow->device(), &groundBufferInfo, nullptr, &mGroundBuffer);
    if (err != VK_SUCCESS)
        qFatal("Failed to create ground buffer: %d", err);

    VkMemoryRequirements groundMemReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(mWindow->device(), mGroundBuffer, &groundMemReq);

    VkMemoryAllocateInfo groundMemAllocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        groundMemReq.size,
        mWindow->hostVisibleMemoryIndex()
    };

    err = mDeviceFunctions->vkAllocateMemory(mWindow->device(), &groundMemAllocInfo, nullptr, &mGroundBufferMemory);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate ground memory: %d", err);

    err = mDeviceFunctions->vkBindBufferMemory(mWindow->device(), mGroundBuffer, mGroundBufferMemory, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind ground buffer memory: %d", err);

    void* groundData;
    err = mDeviceFunctions->vkMapMemory(mWindow->device(), mGroundBufferMemory, 0, groundMemReq.size, 0, &groundData);
    if (err != VK_SUCCESS)
        qFatal("Failed to map ground memory: %d", err);

    memcpy(groundData, groundVertexData, sizeof(groundVertexData));
    mDeviceFunctions->vkUnmapMemory(mWindow->device(), mGroundBufferMemory);

    // Create and set up player buffer
    VkBufferCreateInfo playerBufferInfo = {};
    playerBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    playerBufferInfo.size = sizeof(playerVertexData);
    playerBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    err = mDeviceFunctions->vkCreateBuffer(mWindow->device(), &playerBufferInfo, nullptr, &mPlayerBuffer);
    if (err != VK_SUCCESS)
        qFatal("Failed to create player buffer: %d", err);

    VkMemoryRequirements playerMemReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(mWindow->device(), mPlayerBuffer, &playerMemReq);

    VkMemoryAllocateInfo playerMemAllocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        playerMemReq.size,
        mWindow->hostVisibleMemoryIndex()
    };

    err = mDeviceFunctions->vkAllocateMemory(mWindow->device(), &playerMemAllocInfo, nullptr, &mPlayerBufferMemory);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate player memory: %d", err);

    err = mDeviceFunctions->vkBindBufferMemory(mWindow->device(), mPlayerBuffer, mPlayerBufferMemory, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind player buffer memory: %d", err);

    void* playerData;
    err = mDeviceFunctions->vkMapMemory(mWindow->device(), mPlayerBufferMemory, 0, playerMemReq.size, 0, &playerData);
    if (err != VK_SUCCESS)
        qFatal("Failed to map player memory: %d", err);

    memcpy(playerData, playerVertexData, sizeof(playerVertexData));
    mDeviceFunctions->vkUnmapMemory(mWindow->device(), mPlayerBufferMemory);

    // Create and set up collectible buffer
    VkBufferCreateInfo collectibleBufferInfo = {};
    collectibleBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    collectibleBufferInfo.size = sizeof(collectibleVertexData);
    collectibleBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    err = mDeviceFunctions->vkCreateBuffer(mWindow->device(), &collectibleBufferInfo, nullptr, &mCollectibleBuffer);
    if (err != VK_SUCCESS)
        qFatal("Failed to create collectible buffer: %d", err);

    VkMemoryRequirements collectibleMemReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(mWindow->device(), mCollectibleBuffer, &collectibleMemReq);

    VkMemoryAllocateInfo collectibleMemAllocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        collectibleMemReq.size,
        mWindow->hostVisibleMemoryIndex()
    };

    err = mDeviceFunctions->vkAllocateMemory(mWindow->device(), &collectibleMemAllocInfo, nullptr, &mCollectibleBufferMemory);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate collectible memory: %d", err);

    err = mDeviceFunctions->vkBindBufferMemory(mWindow->device(), mCollectibleBuffer, mCollectibleBufferMemory, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind collectible buffer memory: %d", err);

    void* collectibleData;
    err = mDeviceFunctions->vkMapMemory(mWindow->device(), mCollectibleBufferMemory, 0, collectibleMemReq.size, 0, &collectibleData);
    if (err != VK_SUCCESS)
        qFatal("Failed to map collectible memory: %d", err);

    memcpy(collectibleData, collectibleVertexData, sizeof(collectibleVertexData));
    mDeviceFunctions->vkUnmapMemory(mWindow->device(), mCollectibleBufferMemory);

    // Create NPC buffers - one for each color
    // Buffer 1 - Red NPC
    VkBufferCreateInfo npcBufInfo1 = {};
    npcBufInfo1.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    npcBufInfo1.size = sizeof(npcVertexData1);
    npcBufInfo1.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    err = mDeviceFunctions->vkCreateBuffer(logicalDevice, &npcBufInfo1, nullptr, &mNPCBuffer1);
    VkMemoryRequirements npcMemReq1;
    mDeviceFunctions->vkGetBufferMemoryRequirements(logicalDevice, mNPCBuffer1, &npcMemReq1);
    VkMemoryAllocateInfo npcAllocInfo1 = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        npcMemReq1.size,
        mWindow->hostVisibleMemoryIndex()
    };
    err = mDeviceFunctions->vkAllocateMemory(logicalDevice, &npcAllocInfo1, nullptr, &mNPCBufferMemory1);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate Red NPC buffer memory: %d", err);
    
    err = mDeviceFunctions->vkBindBufferMemory(logicalDevice, mNPCBuffer1, mNPCBufferMemory1, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind Red NPC buffer memory: %d", err);
    
    quint8 *npcVertPtr1;
    err = mDeviceFunctions->vkMapMemory(logicalDevice, mNPCBufferMemory1, 0, npcBufInfo1.size, 0, reinterpret_cast<void **>(&npcVertPtr1));
    memcpy(npcVertPtr1, npcVertexData1, sizeof(npcVertexData1));
    mDeviceFunctions->vkUnmapMemory(logicalDevice, mNPCBufferMemory1);
    
    // Buffer 2 - Green NPC
    VkBufferCreateInfo npcBufInfo2 = {};
    npcBufInfo2.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    npcBufInfo2.size = sizeof(npcVertexData2);
    npcBufInfo2.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    err = mDeviceFunctions->vkCreateBuffer(logicalDevice, &npcBufInfo2, nullptr, &mNPCBuffer2);
    VkMemoryRequirements npcMemReq2;
    mDeviceFunctions->vkGetBufferMemoryRequirements(logicalDevice, mNPCBuffer2, &npcMemReq2);
    VkMemoryAllocateInfo npcAllocInfo2 = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        npcMemReq2.size,
        mWindow->hostVisibleMemoryIndex()
    };
    err = mDeviceFunctions->vkAllocateMemory(logicalDevice, &npcAllocInfo2, nullptr, &mNPCBufferMemory2);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate Green NPC buffer memory: %d", err);
    
    err = mDeviceFunctions->vkBindBufferMemory(logicalDevice, mNPCBuffer2, mNPCBufferMemory2, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind Green NPC buffer memory: %d", err);
    
    quint8 *npcVertPtr2;
    err = mDeviceFunctions->vkMapMemory(logicalDevice, mNPCBufferMemory2, 0, npcBufInfo2.size, 0, reinterpret_cast<void **>(&npcVertPtr2));
    memcpy(npcVertPtr2, npcVertexData2, sizeof(npcVertexData2));
    mDeviceFunctions->vkUnmapMemory(logicalDevice, mNPCBufferMemory2);
    
    // Buffer 3 - Blue NPC
    VkBufferCreateInfo npcBufInfo3 = {};
    npcBufInfo3.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    npcBufInfo3.size = sizeof(npcVertexData3);
    npcBufInfo3.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    err = mDeviceFunctions->vkCreateBuffer(logicalDevice, &npcBufInfo3, nullptr, &mNPCBuffer3);
    VkMemoryRequirements npcMemReq3;
    mDeviceFunctions->vkGetBufferMemoryRequirements(logicalDevice, mNPCBuffer3, &npcMemReq3);
    VkMemoryAllocateInfo npcAllocInfo3 = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        npcMemReq3.size,
        mWindow->hostVisibleMemoryIndex()
    };
    err = mDeviceFunctions->vkAllocateMemory(logicalDevice, &npcAllocInfo3, nullptr, &mNPCBufferMemory3);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate Blue NPC buffer memory: %d", err);
    
    err = mDeviceFunctions->vkBindBufferMemory(logicalDevice, mNPCBuffer3, mNPCBufferMemory3, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind Blue NPC buffer memory: %d", err);
    
    quint8 *npcVertPtr3;
    err = mDeviceFunctions->vkMapMemory(logicalDevice, mNPCBufferMemory3, 0, npcBufInfo3.size, 0, reinterpret_cast<void **>(&npcVertPtr3));
    memcpy(npcVertPtr3, npcVertexData3, sizeof(npcVertexData3));
    mDeviceFunctions->vkUnmapMemory(logicalDevice, mNPCBufferMemory3);
    
    qDebug() << "Created and initialized 3 separate NPC buffers with different colors";

    qDebug() << "Using simpler 'Game Over' notification through window title and debug messages";

    // Create and initialize house walls buffer
    VkBufferCreateInfo houseWallsBufferInfo = {};
    houseWallsBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    houseWallsBufferInfo.size = sizeof(houseWallsVertexData);
    houseWallsBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    err = mDeviceFunctions->vkCreateBuffer(logicalDevice, &houseWallsBufferInfo, nullptr, &mHouseWallsBuffer);
    if (err != VK_SUCCESS)
        qFatal("Failed to create house walls buffer: %d", err);

    VkMemoryRequirements houseWallsMemReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(logicalDevice, mHouseWallsBuffer, &houseWallsMemReq);
    VkMemoryAllocateInfo houseWallsAllocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        houseWallsMemReq.size,
        mWindow->hostVisibleMemoryIndex()
    };
    err = mDeviceFunctions->vkAllocateMemory(logicalDevice, &houseWallsAllocInfo, nullptr, &mHouseWallsBufferMemory);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate house walls memory: %d", err);
    
    err = mDeviceFunctions->vkBindBufferMemory(logicalDevice, mHouseWallsBuffer, mHouseWallsBufferMemory, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind house walls buffer memory: %d", err);
    
    void* houseWallsData;
    err = mDeviceFunctions->vkMapMemory(logicalDevice, mHouseWallsBufferMemory, 0, sizeof(houseWallsVertexData), 0, &houseWallsData);
    if (err != VK_SUCCESS)
        qFatal("Failed to map house walls memory: %d", err);
    memcpy(houseWallsData, houseWallsVertexData, sizeof(houseWallsVertexData));
    mDeviceFunctions->vkUnmapMemory(logicalDevice, mHouseWallsBufferMemory);

    // Create and initialize house door buffer
    VkBufferCreateInfo houseDoorBufferInfo = {};
    houseDoorBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    houseDoorBufferInfo.size = sizeof(houseDoorVertexData);
    houseDoorBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    err = mDeviceFunctions->vkCreateBuffer(logicalDevice, &houseDoorBufferInfo, nullptr, &mHouseDoorBuffer);
    if (err != VK_SUCCESS)
        qFatal("Failed to create house door buffer: %d", err);

    VkMemoryRequirements houseDoorMemReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(logicalDevice, mHouseDoorBuffer, &houseDoorMemReq);
    VkMemoryAllocateInfo houseDoorAllocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        houseDoorMemReq.size,
        mWindow->hostVisibleMemoryIndex()
    };
    err = mDeviceFunctions->vkAllocateMemory(logicalDevice, &houseDoorAllocInfo, nullptr, &mHouseDoorBufferMemory);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate house door memory: %d", err);
    
    err = mDeviceFunctions->vkBindBufferMemory(logicalDevice, mHouseDoorBuffer, mHouseDoorBufferMemory, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind house door buffer memory: %d", err);
    
    void* houseDoorData;
    err = mDeviceFunctions->vkMapMemory(logicalDevice, mHouseDoorBufferMemory, 0, sizeof(houseDoorVertexData), 0, &houseDoorData);
    if (err != VK_SUCCESS)
        qFatal("Failed to map house door memory: %d", err);
    memcpy(houseDoorData, houseDoorVertexData, sizeof(houseDoorVertexData));
    mDeviceFunctions->vkUnmapMemory(logicalDevice, mHouseDoorBufferMemory);

    // Create and initialize house roof buffer
    VkBufferCreateInfo houseRoofBufferInfo = {};
    houseRoofBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    houseRoofBufferInfo.size = sizeof(houseRoofVertexData);
    houseRoofBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    err = mDeviceFunctions->vkCreateBuffer(logicalDevice, &houseRoofBufferInfo, nullptr, &mHouseRoofBuffer);
    if (err != VK_SUCCESS)
        qFatal("Failed to create house roof buffer: %d", err);

    VkMemoryRequirements houseRoofMemReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(logicalDevice, mHouseRoofBuffer, &houseRoofMemReq);
    VkMemoryAllocateInfo houseRoofAllocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        houseRoofMemReq.size,
        mWindow->hostVisibleMemoryIndex()
    };
    err = mDeviceFunctions->vkAllocateMemory(logicalDevice, &houseRoofAllocInfo, nullptr, &mHouseRoofBufferMemory);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate house roof memory: %d", err);
    
    err = mDeviceFunctions->vkBindBufferMemory(logicalDevice, mHouseRoofBuffer, mHouseRoofBufferMemory, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind house roof buffer memory: %d", err);
    
    void* houseRoofData;
    err = mDeviceFunctions->vkMapMemory(logicalDevice, mHouseRoofBufferMemory, 0, sizeof(houseRoofVertexData), 0, &houseRoofData);
    if (err != VK_SUCCESS)
        qFatal("Failed to map house roof memory: %d", err);
    memcpy(houseRoofData, houseRoofVertexData, sizeof(houseRoofVertexData));
    mDeviceFunctions->vkUnmapMemory(logicalDevice, mHouseRoofBufferMemory);

    // Initialize indoor scene resources
    // Create indoor walls buffer
    VkBufferCreateInfo indoorWallsBufferInfo = {};
    indoorWallsBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    indoorWallsBufferInfo.size = sizeof(indoorWallsVertexData);
    indoorWallsBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    err = mDeviceFunctions->vkCreateBuffer(logicalDevice, &indoorWallsBufferInfo, nullptr, &mIndoorWallsBuffer);
    if (err != VK_SUCCESS)
        qFatal("Failed to create indoor walls buffer: %d", err);

    VkMemoryRequirements indoorWallsMemReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(logicalDevice, mIndoorWallsBuffer, &indoorWallsMemReq);
    VkMemoryAllocateInfo indoorWallsAllocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        indoorWallsMemReq.size,
        mWindow->hostVisibleMemoryIndex()
    };
    err = mDeviceFunctions->vkAllocateMemory(logicalDevice, &indoorWallsAllocInfo, nullptr, &mIndoorWallsBufferMemory);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate indoor walls memory: %d", err);
    
    err = mDeviceFunctions->vkBindBufferMemory(logicalDevice, mIndoorWallsBuffer, mIndoorWallsBufferMemory, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind indoor walls buffer memory: %d", err);
    
    void* indoorWallsData;
    err = mDeviceFunctions->vkMapMemory(logicalDevice, mIndoorWallsBufferMemory, 0, sizeof(indoorWallsVertexData), 0, &indoorWallsData);
    if (err != VK_SUCCESS)
        qFatal("Failed to map indoor walls memory: %d", err);
    memcpy(indoorWallsData, indoorWallsVertexData, sizeof(indoorWallsVertexData));
    mDeviceFunctions->vkUnmapMemory(logicalDevice, mIndoorWallsBufferMemory);

    // Create exit door buffer
    VkBufferCreateInfo exitDoorBufferInfo = {};
    exitDoorBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    exitDoorBufferInfo.size = sizeof(exitDoorVertexData);
    exitDoorBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    err = mDeviceFunctions->vkCreateBuffer(logicalDevice, &exitDoorBufferInfo, nullptr, &mExitDoorBuffer);
    if (err != VK_SUCCESS)
        qFatal("Failed to create exit door buffer: %d", err);

    VkMemoryRequirements exitDoorMemReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(logicalDevice, mExitDoorBuffer, &exitDoorMemReq);
    VkMemoryAllocateInfo exitDoorAllocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        exitDoorMemReq.size,
        mWindow->hostVisibleMemoryIndex()
    };
    err = mDeviceFunctions->vkAllocateMemory(logicalDevice, &exitDoorAllocInfo, nullptr, &mExitDoorBufferMemory);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate exit door memory: %d", err);
    
    err = mDeviceFunctions->vkBindBufferMemory(logicalDevice, mExitDoorBuffer, mExitDoorBufferMemory, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind exit door buffer memory: %d", err);
    
    void* exitDoorData;
    err = mDeviceFunctions->vkMapMemory(logicalDevice, mExitDoorBufferMemory, 0, sizeof(exitDoorVertexData), 0, &exitDoorData);
    if (err != VK_SUCCESS)
        qFatal("Failed to map exit door memory: %d", err);
    memcpy(exitDoorData, exitDoorVertexData, sizeof(exitDoorVertexData));
    mDeviceFunctions->vkUnmapMemory(logicalDevice, mExitDoorBufferMemory);

    // Create descriptor sets for the house
    VkDescriptorSetAllocateInfo houseDescSetAllocInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        nullptr,
        mDescriptorPool,
        1,
        &mDescriptorSetLayout
    };

    for (int i = 0; i < concurrentFrameCount; ++i) {
        err = mDeviceFunctions->vkAllocateDescriptorSets(logicalDevice, &houseDescSetAllocInfo, &mHouseDescriptorSet[i]);
        if (err != VK_SUCCESS)
            qFatal("Failed to allocate house descriptor set: %d", err);

        VkWriteDescriptorSet writeDescSet = {
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            nullptr,
            mHouseDescriptorSet[i],
            0, // binding
            0, // arrayElement
            1, // descriptorCount
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            nullptr,
            &mHouseUniformBufferInfo[i],
            nullptr
        };
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 1, &writeDescSet, 0, nullptr);
    }
    
    // Create descriptor sets for the indoor scene
    for (int i = 0; i < concurrentFrameCount; ++i) {
        err = mDeviceFunctions->vkAllocateDescriptorSets(logicalDevice, &houseDescSetAllocInfo, &mIndoorDescriptorSet[i]);
        if (err != VK_SUCCESS)
            qFatal("Failed to allocate indoor descriptor set: %d", err);

        VkWriteDescriptorSet writeDescSet = {
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            nullptr,
            mIndoorDescriptorSet[i],
            0, // binding
            0, // arrayElement
            1, // descriptorCount
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            nullptr,
            &mIndoorUniformBufferInfo[i],
            nullptr
        };
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 1, &writeDescSet, 0, nullptr);
    }

    qDebug("\n ***************************** initResources finished ******************************************* \n");

    getVulkanHWInfo();


    
    // Initialize indoor scene resources
    createIndoorSceneResources();
    
    // Initialize default scene state
    mCurrentScene = 1; // Start in outdoor scene
    
    
}

void RenderWindow::createIndoorSceneResources()
{
    qDebug() << "Creating indoor scene resources...";
    VkDevice device = mWindow->device();
    
    // Indoor room vertices (simple cube room)
    static const float indoorWallsVertexData[] = {
        // Position            // Color (white walls with blue accent)
        // Back wall (negative Z)
        -5.0f, 0.0f, -5.0f,    0.9f, 0.9f, 1.0f,  // Bottom-left
        5.0f, 0.0f, -5.0f,     0.9f, 0.9f, 1.0f,  // Bottom-right
        5.0f, 5.0f, -5.0f,     0.9f, 0.9f, 1.0f,  // Top-right
        -5.0f, 5.0f, -5.0f,    0.9f, 0.9f, 1.0f,  // Top-left
        
        // Front wall (positive Z) with door cutout
        -5.0f, 0.0f, 5.0f,     0.9f, 0.9f, 1.0f,  // Bottom-left
        -1.5f, 0.0f, 5.0f,     0.9f, 0.9f, 1.0f,  // Door left bottom
        -1.5f, 3.0f, 5.0f,     0.9f, 0.9f, 1.0f,  // Door left top
        -5.0f, 5.0f, 5.0f,     0.9f, 0.9f, 1.0f,  // Top-left
        
        // Front wall right of door
        1.5f, 0.0f, 5.0f,      0.9f, 0.9f, 1.0f,  // Door right bottom
        5.0f, 0.0f, 5.0f,      0.9f, 0.9f, 1.0f,  // Bottom-right
        5.0f, 5.0f, 5.0f,      0.9f, 0.9f, 1.0f,  // Top-right
        1.5f, 3.0f, 5.0f,      0.9f, 0.9f, 1.0f,  // Door right top
        
        // Front wall above door
        -1.5f, 3.0f, 5.0f,     0.9f, 0.9f, 1.0f,  // Door left top
        1.5f, 3.0f, 5.0f,      0.9f, 0.9f, 1.0f,  // Door right top
        1.5f, 5.0f, 5.0f,      0.9f, 0.9f, 1.0f,  // Top-right of door
        -1.5f, 5.0f, 5.0f,     0.9f, 0.9f, 1.0f,  // Top-left of door
        
        // Left wall (negative X)
        -5.0f, 0.0f, -5.0f,    0.8f, 0.8f, 1.0f,  // Back bottom
        -5.0f, 0.0f, 5.0f,     0.8f, 0.8f, 1.0f,  // Front bottom
        -5.0f, 5.0f, 5.0f,     0.8f, 0.8f, 1.0f,  // Front top
        -5.0f, 5.0f, -5.0f,    0.8f, 0.8f, 1.0f,  // Back top
        
        // Right wall (positive X)
        5.0f, 0.0f, -5.0f,     0.8f, 0.8f, 1.0f,  // Back bottom
        5.0f, 0.0f, 5.0f,      0.8f, 0.8f, 1.0f,  // Front bottom
        5.0f, 5.0f, 5.0f,      0.8f, 0.8f, 1.0f,  // Front top
        5.0f, 5.0f, -5.0f,     0.8f, 0.8f, 1.0f,  // Back top
        
        // Ceiling (positive Y)
        -5.0f, 5.0f, -5.0f,    0.7f, 0.7f, 1.0f,  // Back-left
        5.0f, 5.0f, -5.0f,     0.7f, 0.7f, 1.0f,  // Back-right
        5.0f, 5.0f, 5.0f,      0.7f, 0.7f, 1.0f,  // Front-right
        -5.0f, 5.0f, 5.0f,     0.7f, 0.7f, 1.0f,  // Front-left
    };
    
    // Exit door vertices
    static const float exitDoorVertexData[] = {
        // Position            // Color (wooden door)
        -1.5f, 0.0f, 5.0f,     0.6f, 0.4f, 0.2f,  // Bottom-left
        1.5f, 0.0f, 5.0f,      0.6f, 0.4f, 0.2f,  // Bottom-right
        1.5f, 3.0f, 5.0f,      0.6f, 0.4f, 0.2f,  // Top-right
        -1.5f, 3.0f, 5.0f,     0.6f, 0.4f, 0.2f,  // Top-left
    };
    
    // Create indoor walls buffer
    VkBufferCreateInfo bufInfo = {};
    bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufInfo.size = sizeof(indoorWallsVertexData);
    bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    
    VkResult err = mDeviceFunctions->vkCreateBuffer(device, &bufInfo, nullptr, &mIndoorWallsBuffer);
    if (err != VK_SUCCESS) {
        qFatal("Failed to create indoor walls buffer: %d", err);
    }
    
    VkMemoryRequirements memReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(device, mIndoorWallsBuffer, &memReq);
    
    VkMemoryAllocateInfo memAllocInfo = {};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = memReq.size;
    memAllocInfo.memoryTypeIndex = mWindow->hostVisibleMemoryIndex();
    
    err = mDeviceFunctions->vkAllocateMemory(device, &memAllocInfo, nullptr, &mIndoorWallsBufferMemory);
    if (err != VK_SUCCESS) {
        qFatal("Failed to allocate indoor walls memory: %d", err);
    }
    
    err = mDeviceFunctions->vkBindBufferMemory(device, mIndoorWallsBuffer, mIndoorWallsBufferMemory, 0);
    if (err != VK_SUCCESS) {
        qFatal("Failed to bind indoor walls buffer memory: %d", err);
    }
    
    void* data;
    err = mDeviceFunctions->vkMapMemory(device, mIndoorWallsBufferMemory, 0, memReq.size, 0, &data);
    if (err != VK_SUCCESS) {
        qFatal("Failed to map indoor walls buffer memory: %d", err);
    }
    memcpy(data, indoorWallsVertexData, sizeof(indoorWallsVertexData));
    mDeviceFunctions->vkUnmapMemory(device, mIndoorWallsBufferMemory);
    
    // Create exit door buffer
    bufInfo.size = sizeof(exitDoorVertexData);
    err = mDeviceFunctions->vkCreateBuffer(device, &bufInfo, nullptr, &mExitDoorBuffer);
    if (err != VK_SUCCESS) {
        qFatal("Failed to create exit door buffer: %d", err);
    }
    
    mDeviceFunctions->vkGetBufferMemoryRequirements(device, mExitDoorBuffer, &memReq);
    memAllocInfo.allocationSize = memReq.size;
    
    err = mDeviceFunctions->vkAllocateMemory(device, &memAllocInfo, nullptr, &mExitDoorBufferMemory);
    if (err != VK_SUCCESS) {
        qFatal("Failed to allocate exit door memory: %d", err);
    }
    
    err = mDeviceFunctions->vkBindBufferMemory(device, mExitDoorBuffer, mExitDoorBufferMemory, 0);
    if (err != VK_SUCCESS) {
        qFatal("Failed to bind exit door buffer memory: %d", err);
    }
    
    err = mDeviceFunctions->vkMapMemory(device, mExitDoorBufferMemory, 0, memReq.size, 0, &data);
    if (err != VK_SUCCESS) {
        qFatal("Failed to map exit door buffer memory: %d", err);
    }
    memcpy(data, exitDoorVertexData, sizeof(exitDoorVertexData));
    mDeviceFunctions->vkUnmapMemory(device, mExitDoorBufferMemory);
    
    // Setup descriptor sets for indoor scene
    const int concurrentFrameCount = mWindow->concurrentFrameCount();
    
    for (int i = 0; i < concurrentFrameCount; ++i) {
        // Allocate indoor descriptor set
        VkDescriptorSetAllocateInfo descSetAllocInfo = {
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            nullptr,
            mDescriptorPool,
            1,
            &mDescriptorSetLayout
        };
        err = mDeviceFunctions->vkAllocateDescriptorSets(device, &descSetAllocInfo, &mIndoorDescriptorSet[i]);
        if (err != VK_SUCCESS) {
            qFatal("Failed to allocate indoor descriptor set: %d", err);
        }
        
        // Update indoor descriptor set
        VkWriteDescriptorSet descWrite = {};
        descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descWrite.dstSet = mIndoorDescriptorSet[i];
        descWrite.descriptorCount = 1;
        descWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descWrite.pBufferInfo = &mIndoorUniformBufferInfo[i];
        mDeviceFunctions->vkUpdateDescriptorSets(device, 1, &descWrite, 0, nullptr);
    }
    
    qDebug() << "Initialized indoor scene resources successfully";
}

void RenderWindow::drawOutdoorScene(VkDevice device, VkCommandBuffer cb, quint8* GPUmemPointer, VkResult& err)
{
    // Draw ground
    QMatrix4x4 groundMatrix;
    groundMatrix.setToIdentity();
    QMatrix4x4 groundMVP = mProjectionMatrix * mViewMatrix * groundMatrix;

    // Update uniform buffer for ground
    VkDeviceSize groundOffset = mUniformBufferInfo[mWindow->currentFrame()].offset;
    err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, groundOffset,
                                        UNIFORM_DATA_SIZE, 0, reinterpret_cast<void **>(&GPUmemPointer));
    memcpy(GPUmemPointer, groundMVP.constData(), 16 * sizeof(float));
    mDeviceFunctions->vkUnmapMemory(device, mBufferMemory);

    // Draw ground
    mDeviceFunctions->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                            &mDescriptorSet[mWindow->currentFrame()], 0, nullptr);
    VkDeviceSize groundVertexOffset = 0;
    mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mGroundBuffer, &groundVertexOffset);
    mDeviceFunctions->vkCmdDraw(cb, 6, 1, 0, 0);  // 6 vertices for ground
    
    qDebug() << "Drew larger ground plane";

    // Draw player cube at its current position
    QMatrix4x4 playerMatrix;
    playerMatrix.setToIdentity();
    playerMatrix.translate(mPlayerPosition);
    
    QMatrix4x4 playerMVP = mProjectionMatrix * mViewMatrix * playerMatrix;

    // Update player's uniform buffer
    VkDeviceSize playerOffset = mPlayerUniformBufferInfo[mWindow->currentFrame()].offset;
    err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, playerOffset,
                                       UNIFORM_DATA_SIZE, 0, reinterpret_cast<void **>(&GPUmemPointer));
    memcpy(GPUmemPointer, playerMVP.constData(), 16 * sizeof(float));
    mDeviceFunctions->vkUnmapMemory(device, mBufferMemory);

    // Draw player cube
    mDeviceFunctions->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                             &mPlayerDescriptorSet[mWindow->currentFrame()], 0, nullptr);
    VkDeviceSize playerVertexOffset = 0;
    mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mPlayerBuffer, &playerVertexOffset);
    mDeviceFunctions->vkCmdDraw(cb, 36, 1, 0, 0);  // 36 vertices for cube

    qDebug() << "Drew player cube at" << mPlayerPosition;

    // Draw collectibles one by one
    int renderedCollectibles = 0;
    qDebug() << "Starting to render" << mCollectibles.size() << "collectibles";
    
    for (int i = 0; i < mCollectibles.size(); ++i) {
        if (!mCollectibles[i].collected) {
            qDebug() << "Rendering collectible" << i << "at position" << mCollectibles[i].position;
            
            // Create matrix for this collectible
            QMatrix4x4 collectibleMatrix;
            collectibleMatrix.setToIdentity();
            collectibleMatrix.translate(mCollectibles[i].position);
            // Make collectibles a bit smaller
            collectibleMatrix.scale(0.4f);
            
            // Update collectible uniform buffer with this collectible's MVP
            QMatrix4x4 collectibleMVP = mProjectionMatrix * mViewMatrix * collectibleMatrix;
            err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, mCollectibleUniformBufferInfo[mWindow->currentFrame()].offset,
                                             UNIFORM_DATA_SIZE, 0, reinterpret_cast<void **>(&GPUmemPointer));
            memcpy(GPUmemPointer, collectibleMVP.constData(), 16 * sizeof(float));
            mDeviceFunctions->vkUnmapMemory(device, mBufferMemory);
            
            // Draw this collectible with correct descriptor set
            mDeviceFunctions->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                                    &mCollectibleDescriptorSet[mWindow->currentFrame()], 0, nullptr);
            VkDeviceSize collectibleVertexOffset = 0;
            mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mCollectibleBuffer, &collectibleVertexOffset);
            mDeviceFunctions->vkCmdDraw(cb, 36, 1, 0, 0);  // 36 vertices for cube
            
            renderedCollectibles++;
        }
    }
    
    qDebug() << "Drew" << renderedCollectibles << "collectibles";

    // Draw NPCs with proper dedicated buffers - one buffer for each NPC with different colors
    int renderedNPCs = 0;
    
    // Draw NPC 1 (Red)
    if (mNPCs.size() > 0) {
        // Create matrix for NPC 1
        QMatrix4x4 npcMatrix;
        npcMatrix.setToIdentity();
        npcMatrix.translate(mNPCs[0].position);
        
        // Make NPCs slightly larger (1.2x) for better visibility
        npcMatrix.scale(1.2f);
        
        // Update uniform buffer with NPC1's MVP
        QMatrix4x4 npcMVP = mProjectionMatrix * mViewMatrix * npcMatrix;
        
        // Use dedicated NPC1 buffer
        VkDeviceSize npcOffset1 = mNPCUniformBufferInfo1[mWindow->currentFrame()].offset;
        err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, npcOffset1,
                                         UNIFORM_DATA_SIZE, 0, reinterpret_cast<void **>(&GPUmemPointer));
        if (err != VK_SUCCESS) {
            qDebug() << "Failed to map memory for NPC1 uniform buffer! Error:" << err;
        } else {
            memcpy(GPUmemPointer, npcMVP.constData(), 16 * sizeof(float));
            mDeviceFunctions->vkUnmapMemory(device, mBufferMemory);
            
            // Use dedicated NPC1 descriptor set
            mDeviceFunctions->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                                   &mNPCDescriptorSet1[mWindow->currentFrame()], 0, nullptr);
            
            // Bind the NPC1 vertex buffer (red)
            VkDeviceSize npcVertexOffset = 0;
            mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mNPCBuffer1, &npcVertexOffset);
            mDeviceFunctions->vkCmdDraw(cb, 36, 1, 0, 0);  // 36 vertices for cube
            
            renderedNPCs++;
            qDebug() << "Drew NPC 0 at position" << mNPCs[0].position << "with unique color";
        }
    }
    
    // Draw NPC 2 (Green)
    if (mNPCs.size() > 1) {
        // Create matrix for NPC 2
        QMatrix4x4 npcMatrix;
        npcMatrix.setToIdentity();
        npcMatrix.translate(mNPCs[1].position);
        
        // Make NPCs slightly larger (1.2x) for better visibility
        npcMatrix.scale(1.2f);
        
        // Update uniform buffer with NPC2's MVP
        QMatrix4x4 npcMVP = mProjectionMatrix * mViewMatrix * npcMatrix;
        
        // Use dedicated NPC2 buffer
        VkDeviceSize npcOffset2 = mNPCUniformBufferInfo2[mWindow->currentFrame()].offset;
        err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, npcOffset2,
                                         UNIFORM_DATA_SIZE, 0, reinterpret_cast<void **>(&GPUmemPointer));
        if (err != VK_SUCCESS) {
            qDebug() << "Failed to map memory for NPC2 uniform buffer! Error:" << err;
        } else {
            memcpy(GPUmemPointer, npcMVP.constData(), 16 * sizeof(float));
            mDeviceFunctions->vkUnmapMemory(device, mBufferMemory);
            
            // Use dedicated NPC2 descriptor set
            mDeviceFunctions->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                                   &mNPCDescriptorSet2[mWindow->currentFrame()], 0, nullptr);
            
            // Bind the NPC2 vertex buffer (green)
            VkDeviceSize npcVertexOffset = 0;
            mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mNPCBuffer2, &npcVertexOffset);
            mDeviceFunctions->vkCmdDraw(cb, 36, 1, 0, 0);  // 36 vertices for cube
            
            renderedNPCs++;
            qDebug() << "Drew NPC 1 at position" << mNPCs[1].position << "with unique color";
        }
    }
    
    // Draw NPC 3 (Blue)
    if (mNPCs.size() > 2) {
        // Create matrix for NPC 3
        QMatrix4x4 npcMatrix;
        npcMatrix.setToIdentity();
        npcMatrix.translate(mNPCs[2].position);
        
        // Make NPCs slightly larger (1.2x) for better visibility
        npcMatrix.scale(1.2f);
        
        // Update uniform buffer with NPC3's MVP
        QMatrix4x4 npcMVP = mProjectionMatrix * mViewMatrix * npcMatrix;
        
        // Use dedicated NPC3 buffer
        VkDeviceSize npcOffset3 = mNPCUniformBufferInfo3[mWindow->currentFrame()].offset;
        err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, npcOffset3,
                                         UNIFORM_DATA_SIZE, 0, reinterpret_cast<void **>(&GPUmemPointer));
        if (err != VK_SUCCESS) {
            qDebug() << "Failed to map memory for NPC3 uniform buffer! Error:" << err;
        } else {
            memcpy(GPUmemPointer, npcMVP.constData(), 16 * sizeof(float));
            mDeviceFunctions->vkUnmapMemory(device, mBufferMemory);
            
            // Use dedicated NPC3 descriptor set
            mDeviceFunctions->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                                   &mNPCDescriptorSet3[mWindow->currentFrame()], 0, nullptr);
            
            // Bind the NPC3 vertex buffer (blue)
            VkDeviceSize npcVertexOffset = 0;
            mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mNPCBuffer3, &npcVertexOffset);
            mDeviceFunctions->vkCmdDraw(cb, 36, 1, 0, 0);  // 36 vertices for cube
            
            renderedNPCs++;
            qDebug() << "Drew NPC 2 at position" << mNPCs[2].position << "with unique color";
        }
    }
    
    qDebug() << "Drew" << renderedNPCs << "NPCs with different colors";

    // Draw game over overlay if player has lost
    if (mGameLost) {
        // Draw a text message on screen (window title will still show you lost)
        qDebug() << "\n*************************************************";
        qDebug() << "*************** GAME OVER! YOU LOST! **************";
        qDebug() << "***    You can press R to restart the game     ***";
        qDebug() << "*************************************************\n";
    }
    
    // Draw house components
    // Create and update house matrix
    QMatrix4x4 houseMatrix;
    houseMatrix.setToIdentity();
    // Position the house at a fixed location
    houseMatrix.translate(mHousePosition);  // Place the house in the corner of the map
    QMatrix4x4 houseMVP = mProjectionMatrix * mViewMatrix * houseMatrix;

    // Update house uniform buffer
    err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, mHouseUniformBufferInfo[mWindow->currentFrame()].offset,
                                     UNIFORM_DATA_SIZE, 0, reinterpret_cast<void **>(&GPUmemPointer));
    if (err != VK_SUCCESS) {
        qDebug() << "Failed to map memory for house uniform buffer! Error:" << err;
    } else {
        memcpy(GPUmemPointer, houseMVP.constData(), 16 * sizeof(float));
        mDeviceFunctions->vkUnmapMemory(device, mBufferMemory);

        // Bind house descriptor set
        mDeviceFunctions->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                               &mHouseDescriptorSet[mWindow->currentFrame()], 0, nullptr);

        // Draw house walls
        VkDeviceSize houseWallsOffset = 0;
        mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mHouseWallsBuffer, &houseWallsOffset);
        mDeviceFunctions->vkCmdDraw(cb, 36, 1, 0, 0);  // 36 vertices for walls (6 faces * 6 vertices)

        // Draw house door
        VkDeviceSize houseDoorOffset = 0;
        mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mHouseDoorBuffer, &houseDoorOffset);
        mDeviceFunctions->vkCmdDraw(cb, 6, 1, 0, 0);  // 6 vertices for door
        
        // Debug output for door state
        if (mDoorOpen) {
            qDebug() << "Drew house door in OPEN position";
        } else {
            qDebug() << "Drew house door in CLOSED position";
        }

        // Draw house roof
        VkDeviceSize houseRoofOffset = 0;
        mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mHouseRoofBuffer, &houseRoofOffset);
        mDeviceFunctions->vkCmdDraw(cb, 12, 1, 0, 0);  // 12 vertices for roof (4 triangles * 3 vertices)

        qDebug() << "Drew house at position" << mHousePosition;
    }
}

void RenderWindow::drawIndoorScene(VkDevice device, VkCommandBuffer cb, quint8* GPUmemPointer, VkResult& err)
{
    // Set a different clear color for indoor scene
    VkClearColorValue indoorClearColor = {{ 0.4f, 0.4f, 0.6f, 1.0f }}; // Light blue-gray indoor lighting
    VkClearAttachment clearAttachment = {};
    clearAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    clearAttachment.colorAttachment = 0;
    clearAttachment.clearValue.color = indoorClearColor;
    
    VkClearRect clearRect = {};
    clearRect.rect.extent.width = mWindow->swapChainImageSize().width();
    clearRect.rect.extent.height = mWindow->swapChainImageSize().height();
    clearRect.layerCount = 1;
    
    // Clear with indoor lighting color
    mDeviceFunctions->vkCmdClearAttachments(cb, 1, &clearAttachment, 1, &clearRect);
    
    // Draw indoor floor
    QMatrix4x4 groundMatrix;
    groundMatrix.setToIdentity();
    // Make the floor dark wood colored by scaling blue component
    groundMatrix.scale(1.0f, 1.0f, 0.5f);
    QMatrix4x4 groundMVP = mProjectionMatrix * mViewMatrix * groundMatrix;

    // Update uniform buffer for ground
    VkDeviceSize groundOffset = mUniformBufferInfo[mWindow->currentFrame()].offset;
    err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, groundOffset,
                                        UNIFORM_DATA_SIZE, 0, reinterpret_cast<void **>(&GPUmemPointer));
    memcpy(GPUmemPointer, groundMVP.constData(), 16 * sizeof(float));
    mDeviceFunctions->vkUnmapMemory(device, mBufferMemory);

    // Draw indoor floor (reusing ground buffer for simplicity)
    mDeviceFunctions->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                            &mDescriptorSet[mWindow->currentFrame()], 0, nullptr);
    VkDeviceSize groundVertexOffset = 0;
    mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mGroundBuffer, &groundVertexOffset);
    mDeviceFunctions->vkCmdDraw(cb, 6, 1, 0, 0);  // 6 vertices for ground
    
    qDebug() << "Drew indoor floor";
    
    // Draw indoor collectible if not collected
    if (!mIndoorCollectible.collected) {
        // Create matrix for this collectible
        QMatrix4x4 collectibleMatrix;
        collectibleMatrix.setToIdentity();
        collectibleMatrix.translate(mIndoorCollectible.position);
        // Make collectibles a bit smaller and shinier
        collectibleMatrix.scale(0.5f);
        
        // Update collectible uniform buffer
        QMatrix4x4 collectibleMVP = mProjectionMatrix * mViewMatrix * collectibleMatrix;
        err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, mCollectibleUniformBufferInfo[mWindow->currentFrame()].offset,
                                         UNIFORM_DATA_SIZE, 0, reinterpret_cast<void **>(&GPUmemPointer));
        memcpy(GPUmemPointer, collectibleMVP.constData(), 16 * sizeof(float));
        mDeviceFunctions->vkUnmapMemory(device, mBufferMemory);
        
        // Draw the indoor collectible with golden color
        mDeviceFunctions->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                                &mCollectibleDescriptorSet[mWindow->currentFrame()], 0, nullptr);
        VkDeviceSize collectibleVertexOffset = 0;
        mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mCollectibleBuffer, &collectibleVertexOffset);
        mDeviceFunctions->vkCmdDraw(cb, 36, 1, 0, 0);  // 36 vertices for cube
        
        qDebug() << "Drew special indoor collectible at" << mIndoorCollectible.position;
    }

    // Draw player cube at its current position inside the house
    QMatrix4x4 playerMatrix;
    playerMatrix.setToIdentity();
    playerMatrix.translate(mPlayerPosition);
    
    QMatrix4x4 playerMVP = mProjectionMatrix * mViewMatrix * playerMatrix;

    // Update player's uniform buffer
    VkDeviceSize playerOffset = mPlayerUniformBufferInfo[mWindow->currentFrame()].offset;
    err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, playerOffset,
                                       UNIFORM_DATA_SIZE, 0, reinterpret_cast<void **>(&GPUmemPointer));
    memcpy(GPUmemPointer, playerMVP.constData(), 16 * sizeof(float));
    mDeviceFunctions->vkUnmapMemory(device, mBufferMemory);

    // Draw player cube
    mDeviceFunctions->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                             &mPlayerDescriptorSet[mWindow->currentFrame()], 0, nullptr);
    VkDeviceSize playerVertexOffset = 0;
    mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mPlayerBuffer, &playerVertexOffset);
    mDeviceFunctions->vkCmdDraw(cb, 36, 1, 0, 0);  // 36 vertices for cube

    qDebug() << "Drew player cube at" << mPlayerPosition << "inside house";

    // Draw a helpful message to instruct player how to exit
    qDebug() << "\n*************************************************";
    qDebug() << "***       YOU ARE INSIDE THE HOUSE           ***";
    qDebug() << "***    Press 'E' key to exit the house       ***";
    qDebug() << "*************************************************\n";
}

void RenderWindow::startNextFrame()
{
    // Update NPC positions
    updateNPCs();
    
    // Check for NPC collisions - reset player if hit
    if (checkNPCCollision()) {
        // Reset player to starting position (0,0,0)
        mPlayerPosition = QVector3D(0.0f, 0.0f, 0.0f);
        
        // Set the game status to indicate player lost
        mGameLost = true;
        
        // Force UI update to show "You lose!" message
        if (VulkanWindow* vulkanWindow = qobject_cast<VulkanWindow*>(mWindow)) {
            vulkanWindow->updateGameStatus(VulkanWindow::GameStatus::Lost);
        }
    }
    
    // Only check collectibles if game is still ongoing
    if (!mGameLost && !mGameWon) {
        // Check collectible collisions - different for each scene
        if (mCurrentScene == 1) {
            // In outdoor scene - check door proximity and outdoor collectibles
            checkDoorProximity();
            checkCollectibleCollisions();
        } else {
            // In indoor scene - check indoor collectible
            checkIndoorCollectibleCollision();
        }
    }
    
    // Show game state messages
    if (mGameLost) {
        // You lose message is shown in checkNPCCollision()
    } else if (mGameWon) {
        // Display win message each frame to ensure it's visible
        qDebug() << "\n*************************************************";
        qDebug() << "***               YOU WON!                    ***";
        qDebug() << "***     All collectibles have been found!     ***";
        qDebug() << "*************************************************\n";
    }
    
    // Always check for win condition (in case it was missed during collection)
    if (!mGameLost && mCollectedCount > 0) {
        checkGameWinCondition();
    }
    
    // SIMPLIFIED CAMERA - more angled view to see the scene better
    const QVector3D cameraPos(0.0f, 20.0f, 20.0f);  // Position higher and back to see more
    const QVector3D cameraTarget(0.0f, 0.0f, 0.0f); // Look at center
    
    // Create view matrix with angled camera
    QMatrix4x4 viewMatrix;
    viewMatrix.lookAt(cameraPos, cameraTarget, QVector3D(0.0f, 1.0f, 0.0f)); // Use standard Y up direction

    // Create projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, mAspectRatio, 0.1f, 100.0f);

    mViewMatrix = viewMatrix;
    mProjectionMatrix = projectionMatrix;

    VkDevice device = mWindow->device();
    VkCommandBuffer cb = mWindow->currentCommandBuffer();
    const QSize sz = mWindow->swapChainImageSize();

    // Clear screen
    VkClearColorValue clearColor = {{ 0.0f, 1.0f, 0.0f, 1.0f }}; // Changed to bright green for debugging
    VkClearDepthStencilValue clearDS = { 1.0f, 0 };
    VkClearValue clearValues[3];
    clearValues[0].color = clearValues[2].color = clearColor;
    clearValues[1].depthStencil = clearDS;

    VkRenderPassBeginInfo rpBeginInfo = {};
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = mWindow->defaultRenderPass();
    rpBeginInfo.framebuffer = mWindow->currentFramebuffer();
    rpBeginInfo.renderArea.extent.width = sz.width();
    rpBeginInfo.renderArea.extent.height = sz.height();
    rpBeginInfo.clearValueCount = 3;
    rpBeginInfo.pClearValues = clearValues;

    VkCommandBuffer cmdBuf = mWindow->currentCommandBuffer();
    mDeviceFunctions->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Set viewport and scissor
    VkViewport viewport = {};
    viewport.width = sz.width();
    viewport.height = sz.height();
    viewport.minDepth = 0;
    viewport.maxDepth = 1;
    mDeviceFunctions->vkCmdSetViewport(cb, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.extent.width = sz.width();
    scissor.extent.height = sz.height();
    mDeviceFunctions->vkCmdSetScissor(cb, 0, 1, &scissor);

    // Bind pipeline once for all draws
    mDeviceFunctions->vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

    // Initialize memory pointer and error variable once at the beginning of the function
    quint8* GPUmemPointer = nullptr;
    VkResult err = VK_SUCCESS;

    // Draw the appropriate scene based on current scene value
    if (mCurrentScene == 1) {
        // Draw outdoor scene
        drawOutdoorScene(device, cb, GPUmemPointer, err);
    } else {
        // Draw indoor scene
        drawIndoorScene(device, cb, GPUmemPointer, err);
    }
    
    // Debug output to confirm render pass status
    qDebug() << "Ending render pass and submitting draw commands...";
    
    // End render pass
    mDeviceFunctions->vkCmdEndRenderPass(cmdBuf);
    
    // Debug output to confirm submission
    qDebug() << "Render pass ended, submitting frame...";
    
    mWindow->frameReady();
    mWindow->requestUpdate();
}


VkShaderModule RenderWindow::createShader(const QString &name)
{
    //This uses Qt's own file opening and resource system
    //We probably will replace it with pure C++ when expanding the program
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Failed to read shader %s", qPrintable(name));
        return VK_NULL_HANDLE;
    }
    QByteArray blob = file.readAll();
    file.close();

    VkShaderModuleCreateInfo shaderInfo;
    memset(&shaderInfo, 0, sizeof(shaderInfo));
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = blob.size();
    shaderInfo.pCode = reinterpret_cast<const uint32_t *>(blob.constData());
    VkShaderModule shaderModule;
    VkResult err = mDeviceFunctions->vkCreateShaderModule(mWindow->device(), &shaderInfo, nullptr, &shaderModule);
    if (err != VK_SUCCESS) {
        qWarning("Failed to create shader module: %d", err);
        return VK_NULL_HANDLE;
    }

    return shaderModule;
}

void RenderWindow::getVulkanHWInfo()
{
    qDebug("\n ***************************** Vulkan Hardware Info ******************************************* \n");
    QVulkanInstance *inst = mWindow->vulkanInstance();
    mDeviceFunctions = inst->deviceFunctions(mWindow->device());

    QString info;
    info += QString::asprintf("Number of physical devices: %d\n", int(mWindow->availablePhysicalDevices().count()));

    QVulkanFunctions *f = inst->functions();
    VkPhysicalDeviceProperties props;
    f->vkGetPhysicalDeviceProperties(mWindow->physicalDevice(), &props);
    info += QString::asprintf("Active physical device name: '%s' version %d.%d.%d\nAPI version %d.%d.%d\n",
                              props.deviceName,
                              VK_VERSION_MAJOR(props.driverVersion), VK_VERSION_MINOR(props.driverVersion),
                              VK_VERSION_PATCH(props.driverVersion),
                              VK_VERSION_MAJOR(props.apiVersion), VK_VERSION_MINOR(props.apiVersion),
                              VK_VERSION_PATCH(props.apiVersion));

    info += QStringLiteral("Supported instance layers:\n");
    for (const QVulkanLayer &layer : inst->supportedLayers())
        info += QString::asprintf("    %s v%u\n", layer.name.constData(), layer.version);
    info += QStringLiteral("Enabled instance layers:\n");
    for (const QByteArray &layer : inst->layers())
        info += QString::asprintf("    %s\n", layer.constData());

    info += QStringLiteral("Supported instance extensions:\n");
    for (const QVulkanExtension &ext : inst->supportedExtensions())
        info += QString::asprintf("    %s v%u\n", ext.name.constData(), ext.version);
    info += QStringLiteral("Enabled instance extensions:\n");
    for (const QByteArray &ext : inst->extensions())
        info += QString::asprintf("    %s\n", ext.constData());

    info += QString::asprintf("Color format: %u\nDepth-stencil format: %u\n",
                              mWindow->colorFormat(), mWindow->depthStencilFormat());

    info += QStringLiteral("Supported sample counts:");
    const QList<int> sampleCounts = mWindow->supportedSampleCounts();
    for (int count : sampleCounts)
        info += QLatin1Char(' ') + QString::number(count);
    info += QLatin1Char('\n');

    qDebug(info.toUtf8().constData());
    qDebug("\n ***************************** Vulkan Hardware Info finished ******************************************* \n");
}

void RenderWindow::releaseSwapChainResources()
{
    qDebug("\n ***************************** releaseSwapChainResources ******************************************* \n");
    // Nothing specific to release in swap chain resources
    // This method is called when the swap chain is being recreated,
    // such as when the window is resized
}

void RenderWindow::releaseResources()
{
    qDebug("\n ***************************** releaseResources ******************************************* \n");

    VkDevice device = mWindow->device();

    if (mPipeline) {
        mDeviceFunctions->vkDestroyPipeline(device, mPipeline, nullptr);
        mPipeline = VK_NULL_HANDLE;
    }

    if (mPipelineLayout) {
        mDeviceFunctions->vkDestroyPipelineLayout(device, mPipelineLayout, nullptr);
        mPipelineLayout = VK_NULL_HANDLE;
    }

    if (mPipelineCache) {
        mDeviceFunctions->vkDestroyPipelineCache(device, mPipelineCache, nullptr);
        mPipelineCache = VK_NULL_HANDLE;
    }

    if (mDescriptorSetLayout) {
        mDeviceFunctions->vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);
        mDescriptorSetLayout = VK_NULL_HANDLE;
    }

    if (mDescriptorPool) {
        // Note: Destroying the descriptor pool automatically frees all descriptor sets
        // allocated from it (both mDescriptorSet and mPlayerDescriptorSet)
        mDeviceFunctions->vkDestroyDescriptorPool(device, mDescriptorPool, nullptr);
        mDescriptorPool = VK_NULL_HANDLE;
    }

    if (mBuffer) {
        mDeviceFunctions->vkDestroyBuffer(device, mBuffer, nullptr);
        mBuffer = VK_NULL_HANDLE;
    }

    if (mBufferMemory) {
        mDeviceFunctions->vkFreeMemory(device, mBufferMemory, nullptr);
        mBufferMemory = VK_NULL_HANDLE;
    }

    if (mGroundBuffer) {
        mDeviceFunctions->vkDestroyBuffer(mWindow->device(), mGroundBuffer, nullptr);
        mGroundBuffer = VK_NULL_HANDLE;
    }

    if (mGroundBufferMemory) {
        mDeviceFunctions->vkFreeMemory(mWindow->device(), mGroundBufferMemory, nullptr);
        mGroundBufferMemory = VK_NULL_HANDLE;
    }

    if (mPlayerBuffer) {
        mDeviceFunctions->vkDestroyBuffer(mWindow->device(), mPlayerBuffer, nullptr);
        mPlayerBuffer = VK_NULL_HANDLE;
    }

    if (mPlayerBufferMemory) {
        mDeviceFunctions->vkFreeMemory(mWindow->device(), mPlayerBufferMemory, nullptr);
        mPlayerBufferMemory = VK_NULL_HANDLE;
    }

    if (mCollectibleBuffer) {
        mDeviceFunctions->vkDestroyBuffer(mWindow->device(), mCollectibleBuffer, nullptr);
        mCollectibleBuffer = VK_NULL_HANDLE;
    }

    if (mCollectibleBufferMemory) {
        mDeviceFunctions->vkFreeMemory(mWindow->device(), mCollectibleBufferMemory, nullptr);
        mCollectibleBufferMemory = VK_NULL_HANDLE;
    }

    if (mNPCBuffer) {
        mDeviceFunctions->vkDestroyBuffer(device, mNPCBuffer, nullptr);
        mNPCBuffer = VK_NULL_HANDLE;
    }

    if (mNPCBufferMemory) {
        mDeviceFunctions->vkFreeMemory(device, mNPCBufferMemory, nullptr);
        mNPCBufferMemory = VK_NULL_HANDLE;
    }
    
    // Free new NPC colored buffers
    if (mNPCBuffer1) {
        mDeviceFunctions->vkDestroyBuffer(device, mNPCBuffer1, nullptr);
        mNPCBuffer1 = VK_NULL_HANDLE;
    }

    if (mNPCBufferMemory1) {
        mDeviceFunctions->vkFreeMemory(device, mNPCBufferMemory1, nullptr);
        mNPCBufferMemory1 = VK_NULL_HANDLE;
    }
    
    if (mNPCBuffer2) {
        mDeviceFunctions->vkDestroyBuffer(device, mNPCBuffer2, nullptr);
        mNPCBuffer2 = VK_NULL_HANDLE;
    }

    if (mNPCBufferMemory2) {
        mDeviceFunctions->vkFreeMemory(device, mNPCBufferMemory2, nullptr);
        mNPCBufferMemory2 = VK_NULL_HANDLE;
    }
    
    if (mNPCBuffer3) {
        mDeviceFunctions->vkDestroyBuffer(device, mNPCBuffer3, nullptr);
        mNPCBuffer3 = VK_NULL_HANDLE;
    }

    if (mNPCBufferMemory3 != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(mWindow->device(), mNPCBufferMemory3, nullptr);
        mNPCBufferMemory3 = VK_NULL_HANDLE;
    }

    // Free house buffers
    if (mHouseWallsBuffer) {
        mDeviceFunctions->vkDestroyBuffer(device, mHouseWallsBuffer, nullptr);
        mHouseWallsBuffer = VK_NULL_HANDLE;
    }

    if (mHouseWallsBufferMemory) {
        mDeviceFunctions->vkFreeMemory(device, mHouseWallsBufferMemory, nullptr);
        mHouseWallsBufferMemory = VK_NULL_HANDLE;
    }

    if (mHouseDoorBuffer) {
        mDeviceFunctions->vkDestroyBuffer(device, mHouseDoorBuffer, nullptr);
        mHouseDoorBuffer = VK_NULL_HANDLE;
    }

    if (mHouseDoorBufferMemory) {
        mDeviceFunctions->vkFreeMemory(device, mHouseDoorBufferMemory, nullptr);
        mHouseDoorBufferMemory = VK_NULL_HANDLE;
    }

    if (mHouseRoofBuffer) {
        mDeviceFunctions->vkDestroyBuffer(device, mHouseRoofBuffer, nullptr);
        mHouseRoofBuffer = VK_NULL_HANDLE;
    }

    if (mHouseRoofBufferMemory) {
        mDeviceFunctions->vkFreeMemory(device, mHouseRoofBufferMemory, nullptr);
        mHouseRoofBufferMemory = VK_NULL_HANDLE;
    }

    // Free house descriptor sets
    if (mHouseDescriptorSet[0]) {
        mDeviceFunctions->vkFreeDescriptorSets(device, mDescriptorPool, 1, mHouseDescriptorSet);
        mHouseDescriptorSet[0] = VK_NULL_HANDLE;
    }

    qDebug("\n ***************************** releaseResources finished ******************************************* \n");
}

void RenderWindow::initializeCollectibles()
{
    // Clear any existing collectibles
    mCollectibles.clear();
    mCollectedCount = 0;

    // Create collectibles in a grid with HIGHER POSITION
    // Making them clearly visible on the larger ground plane
    mCollectibles.append(Collectible(QVector3D(-6.0f, 1.5f, -6.0f)));   // Top left
    mCollectibles.append(Collectible(QVector3D(0.0f, 1.5f, -6.0f)));    // Top center
    mCollectibles.append(Collectible(QVector3D(6.0f, 1.5f, -6.0f)));    // Top right
    mCollectibles.append(Collectible(QVector3D(-6.0f, 1.5f, 6.0f)));    // Bottom left
    mCollectibles.append(Collectible(QVector3D(0.0f, 1.5f, 6.0f)));     // Bottom center
    mCollectibles.append(Collectible(QVector3D(6.0f, 1.5f, 6.0f)));     // Bottom right

    // Reset the indoor collectible to not collected
    mIndoorCollectible.collected = false;

    qDebug() << "Initialized 6 outdoor and 1 indoor collectibles";
}

void RenderWindow::checkCollectibleCollisions()
{
    const float collectionDistance = 2.0f; // Increased distance for easier collection
    bool collectedAny = false;
    
    for (int i = 0; i < mCollectibles.size(); ++i) {
        if (!mCollectibles[i].collected) {
            // For collision, only check X and Z coordinates since Y is different by design
            QVector3D playerXZ(mPlayerPosition.x(), 0.0f, mPlayerPosition.z());
            QVector3D collectibleXZ(mCollectibles[i].position.x(), 0.0f, mCollectibles[i].position.z());
            
            float distance = (playerXZ - collectibleXZ).length();
            
            if (distance < collectionDistance) {
                // Collect the item
                mCollectibles[i].collected = true;
                mCollectedCount++;
                collectedAny = true;
                qDebug() << "COLLECTED: item at position" << mCollectibles[i].position << "!"
                         << mCollectedCount << "of" << getTotalCollectibles() << "collected";
                
                // Check if all collectibles (including indoor) are collected
                checkGameWinCondition();
            }
        }
    }
    
    if (collectedAny) {
        // Force an immediate update of the UI when a collectible is collected
        if (mWindow) {
            mWindow->requestUpdate();
        }
    }
}

void RenderWindow::updateUniformBuffer(VkDescriptorBufferInfo &bufferInfo, const QMatrix4x4 &matrix)
{
    VkDevice device = mWindow->device();
    QMatrix4x4 mvp = mProjectionMatrix * mViewMatrix * matrix;
    
    // Check if bufferInfo is valid
    if (!bufferInfo.buffer) {
        qDebug() << "WARNING: Invalid buffer in updateUniformBuffer - buffer is null";
        return;
    }
    
    quint8* GPUmemPointer;
    VkResult err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, bufferInfo.offset,
                                             UNIFORM_DATA_SIZE, 0, reinterpret_cast<void **>(&GPUmemPointer));
    if (err != VK_SUCCESS) {
        qDebug() << "Failed to map memory for uniform buffer! Error:" << err;
        return;
    }
    
    memcpy(GPUmemPointer, mvp.constData(), 16 * sizeof(float));
    mDeviceFunctions->vkUnmapMemory(device, mBufferMemory);
}

void RenderWindow::initializeNPCs()
{
    // Clear any existing NPCs
    mNPCs.clear();

    // Create three NPCs with distinct patrol paths, different heights and colors
    // Use much more extreme height differences for better visibility
    
    // NPC 1: Higher position (red) - patrols horizontally top of map
    mNPCs.append(PatrolEnemy(
        QVector3D(-8.0f, 2.0f, -5.0f),   // Start at top left, much higher position
        QVector3D(8.0f, 2.0f, -5.0f),    // Patrol to top right
        0.03f                           // Slower speed for visibility
    ));
    
    // NPC 2: Middle height (green) - patrols horizontally bottom of map
    mNPCs.append(PatrolEnemy(
        QVector3D(-8.0f, 0.5f, 5.0f),   // Start at bottom left, lower than player
        QVector3D(8.0f, 0.5f, 5.0f),    // Patrol to bottom right
        0.04f                           // Medium speed
    ));
    
    // NPC 3: Highest position (blue) - patrols vertically on right side
    mNPCs.append(PatrolEnemy(
        QVector3D(5.0f, 3.0f, -8.0f),   // Start at back right, much higher position
        QVector3D(5.0f, 3.0f, 8.0f),    // Patrol to front right
        0.05f                           // Faster speed
    ));
    
    qDebug() << "\n*** INITIALIZED" << mNPCs.size() << "NPCS WITH IMPROVED VISIBILITY ***";
    qDebug() << " - NPC 1 (TOP): Red, height 2.0, patrols across top";
    qDebug() << " - NPC 2 (BOTTOM): Green, height 0.5, patrols across bottom";
    qDebug() << " - NPC 3 (RIGHT): Blue, height 3.0, patrols along right side\n";
    
    // Detailed debug output for NPC positions
    for (int i = 0; i < mNPCs.size(); ++i) {
        qDebug() << "NPC" << i << "patrolling from" << mNPCs[i].pointA << "to" << mNPCs[i].pointB
                 << "with speed" << mNPCs[i].speed;
    }
}

void RenderWindow::updateNPCs()
{
    for (int i = 0; i < mNPCs.size(); ++i) {
        // Update NPC position along its patrol route
        mNPCs[i].updatePosition();
    }
}

bool RenderWindow::checkNPCCollision()
{
    const float collisionDistance = 0.9f; // Reduced for more precise detection
    
    for (int i = 0; i < mNPCs.size(); ++i) {
        // For collision, only check X and Z coordinates since Y is different by design
        QVector3D playerXZ(mPlayerPosition.x(), 0.0f, mPlayerPosition.z());
        QVector3D npcXZ(mNPCs[i].position.x(), 0.0f, mNPCs[i].position.z());
        
        float distance = (playerXZ - npcXZ).length();
        
        // Enhanced debug output to track distances
        if (distance < 3.0f) {
            qDebug() << "NEAR NPC" << i << ": Player at" << playerXZ 
                     << "is" << distance << "units from NPC at" << npcXZ
                     << "(collision occurs at < " << collisionDistance << ")";
        }
        
        if (distance < collisionDistance) {
            QString npcColor;
            switch(i) {
                case 0: npcColor = "RED"; break;
                case 1: npcColor = "GREEN"; break;
                case 2: npcColor = "BLUE"; break;
                default: npcColor = "UNKNOWN"; break;
            }
            
            // Set game lost state
            mGameLost = true;
            
            // Print clear game over message
            qDebug() << "\n*************************************************";
            qDebug() << "*************** GAME OVER! YOU LOST! **************";
            qDebug() << "*** You collided with the" << npcColor << "NPC!" << "***";
            qDebug() << "*** Your final position:" << mPlayerPosition << "***";
            qDebug() << "*** Press R to restart the game ***";
            qDebug() << "*************************************************\n";
            
            return true; // Player hit an NPC
        }
    }
    
    return false; // No collision
}

// Helper function to find memory type index
static uint32_t getMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties& memProperties, 
                                 uint32_t typeBits, 
                                 VkMemoryPropertyFlags properties) 
{
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeBits & (1 << i)) && 
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return ~0u;  // Invalid index
}

void RenderWindow::checkDoorProximity()
{
    // Calculate the actual door position based on house position
    QVector3D doorPosition = mHousePosition + QVector3D(0.0f, 0.0f, 3.0f); // Door is at the front of the house
    
    // Calculate distance between player and door
    float distance = (mPlayerPosition - doorPosition).length();
    
    // Debug output
    qDebug() << "Distance to door:" << distance << "Player position:" << mPlayerPosition << "Door position:" << doorPosition;
    
    // Update door state based on proximity
    if (distance < mDoorOpenDistance) {
        if (!mDoorOpen) {
            qDebug() << "Player is close to door - opening!";
            updateDoorState(true);
        }
        
        // Check if player is trying to enter the house (by pressing a specific key or moving through door)
        checkHouseEntry(doorPosition);
    } else {
        if (mDoorOpen) {
            qDebug() << "Player moved away from door - closing!";
            updateDoorState(false);
        }
    }
}

void RenderWindow::checkHouseEntry(const QVector3D& doorPosition)
{
    // Check if the player is moving through the door into the house
    // This is determined by checking if:
    // 1. The door is open 
    // 2. The player is at the doorway
    
    if (!mDoorOpen) {
        qDebug() << "Door is closed, cannot enter house";
        return; // Door must be open to enter
    }
    
    // Calculate player position relative to door
    QVector3D relativePos = mPlayerPosition - doorPosition;
    
    // Debug output to track player position relative to door
    qDebug() << "Player relative to door:" << relativePos 
             << "Distance:" << relativePos.length() 
             << "X offset:" << std::abs(relativePos.x());
    
    // Check if player is within entry zone 
    // (within small distance from door and aligned with doorway)
    float entryDistance = 1.5f; // Increased for easier detection
    float doorwayWidth = 1.5f;  // Increased for easier entry
    
    // If player is close to door position and roughly aligned with door
    if (relativePos.length() < entryDistance && 
        std::abs(relativePos.x()) < doorwayWidth &&
        relativePos.z() < 0.5f && relativePos.z() > -0.5f) { // Near the door plane
        
        qDebug() << "*** PLAYER ENTERED HOUSE - TRANSITIONING TO SCENE 2! ***";
        // Force scene transition
        transitionToScene2();
    }
}

void RenderWindow::transitionToScene2()
{
    // Save player state from scene 1
    mScene1PlayerPosition = mPlayerPosition;
    
    // Set up Scene 2 (inside house)
    mCurrentScene = 2;
    
    // Reset player position for inside the house
    mPlayerPosition = QVector3D(0.0f, 0.0f, 0.0f); // Center of the house
    
    // Reset view and state for Scene 2
    qDebug() << "Transitioned to Scene 2 (inside house)";
    
    // Request a render update
    if (mWindow) {
        mWindow->requestUpdate();
    }
}

void RenderWindow::transitionToScene1()
{
    // Set up Scene 1 (outside)
    mCurrentScene = 1;
    
    // Restore player position from where they entered the house
    // Place them just outside the door
    QVector3D doorPosition = mHousePosition + QVector3D(0.0f, 0.0f, 3.0f);
    mPlayerPosition = doorPosition + QVector3D(0.0f, 0.0f, 1.0f); // Just outside the door
    
    // Reset view and state for Scene 1
    qDebug() << "Transitioned to Scene 1 (outside)";
    
    // Request a render update
    if (mWindow) {
        mWindow->requestUpdate();
    }
}

void RenderWindow::updateDoorState(bool open)
{
    if (mDoorOpen == open) {
        return; // No change needed
    }
    
    mDoorOpen = open;
    
    // Update door buffer with appropriate vertex data
    VkDevice device = mWindow->device();
    
    // Map the door buffer memory
    void* doorData;
    VkResult err = mDeviceFunctions->vkMapMemory(device, mHouseDoorBufferMemory, 0, 
                                                sizeof(houseDoorVertexData), 0, &doorData);
    if (err != VK_SUCCESS) {
        qDebug() << "Failed to map door memory! Error:" << err;
        return;
    }
    
    // Copy the appropriate vertex data based on door state
    if (mDoorOpen) {
        memcpy(doorData, houseDoorOpenVertexData, sizeof(houseDoorOpenVertexData));
        qDebug() << "Door opened - updated vertex data";
    } else {
        memcpy(doorData, houseDoorVertexData, sizeof(houseDoorVertexData));
        qDebug() << "Door closed - updated vertex data";
    }
    
    // Unmap the memory
    mDeviceFunctions->vkUnmapMemory(device, mHouseDoorBufferMemory);
    
    // Request a redraw to show the updated door state
    if (mWindow) {
        mWindow->requestUpdate();
    }
}

void RenderWindow::tryExitHouse()
{
    // Only works in Scene 2
    if (mCurrentScene != 2) {
        qDebug() << "Not inside house, can't exit";
        return;
    }
    
    qDebug() << "\n*************************************************";
    qDebug() << "***       EXITING HOUSE - BACK OUTSIDE!       ***";
    qDebug() << "*************************************************\n";
    
    // Transition back to outdoor scene
    transitionToScene1();
}

void RenderWindow::initSwapChainResources()
{
    qDebug("initSwapChainResources: Window size is %dx%d", mWindow->width(), mWindow->height());
    
    // Save aspect ratio for projection matrix
    mAspectRatio = float(mWindow->swapChainImageSize().width()) / float(mWindow->swapChainImageSize().height());
    
    // No other resources to initialize in this demo
}

void RenderWindow::checkIndoorCollectibleCollision()
{
    // Only check in Scene 2 (indoor)
    if (mCurrentScene != 2) {
        return;
    }
    
    // Skip if already collected
    if (mIndoorCollectible.collected) {
        return;
    }
    
    // Calculate distance between player and collectible (in XZ plane)
    QVector3D playerPos2D(mPlayerPosition.x(), 0.0f, mPlayerPosition.z());
    QVector3D collectiblePos2D(mIndoorCollectible.position.x(), 0.0f, mIndoorCollectible.position.z());
    
    float distance = (playerPos2D - collectiblePos2D).length();
    
    // Collection radius
    const float COLLECT_RADIUS = 1.0f;
    
    if (distance < COLLECT_RADIUS) {
        // Mark as collected
        mIndoorCollectible.collected = true;
        
        // Increment count (using the same counter as outdoor collectibles)
        mCollectedCount++;
        
        qDebug() << "*** SPECIAL INDOOR COLLECTIBLE COLLECTED! ***";
        qDebug() << "Total collectibles:" << mCollectedCount << "of" << getTotalCollectibles();
        
        // Check if all collectibles are collected
        checkGameWinCondition();
        
        // Request update to refresh rendering
        if (mWindow) {
            mWindow->requestUpdate();
        }
    }
}

void RenderWindow::checkGameWinCondition()
{
    // Skip if already won or lost
    if (mGameWon || mGameLost) {
        return;
    }
    
    // Debug output to track what's happening
    qDebug() << "CHECKING WIN CONDITION: Collected:" << mCollectedCount << "Total:" << getTotalCollectibles();
    
    // Check if all collectibles are collected
    if (mCollectedCount == getTotalCollectibles()) {
        // Set the game won flag
        mGameWon = true;
        
        // Display prominent win message
        qDebug() << "\n*************************************************";
        qDebug() << "***               YOU WON!                    ***";
        qDebug() << "***     All collectibles have been found!     ***";
        qDebug() << "*************************************************\n";
        
        // Update game UI
        if (VulkanWindow* vulkanWindow = qobject_cast<VulkanWindow*>(mWindow)) {
            qDebug() << "Updating window status to Won!";
            vulkanWindow->updateGameStatus(VulkanWindow::GameStatus::Won);
        } else {
            qDebug() << "ERROR: Could not cast to VulkanWindow!";
        }
    }
}






