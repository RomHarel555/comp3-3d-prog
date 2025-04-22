#include "RenderWindow.h"
#include <QVulkanFunctions>
#include <QFile>
#include <QFileInfo>  // Added for QFileInfo class
#include <QDir>       // Added for QDir class
#include <QDebug>
#include "VulkanWindow.h"

// ENLARGED ground vertex data (10x10 plane instead of 5x5)
static float groundVertexData[] = {
    // Position            // Color              // Texture coords
    -10.0f,  0.0f, -10.0f,   0.3f, 0.3f, 0.3f,   0.0f, 0.0f,  // Bottom-left
    -10.0f,  0.0f,  10.0f,   0.3f, 0.3f, 0.3f,   0.0f, 1.0f,  // Top-left
     10.0f,  0.0f, -10.0f,   0.3f, 0.3f, 0.3f,   1.0f, 0.0f,  // Bottom-right

    -10.0f,  0.0f,  10.0f,   0.3f, 0.3f, 0.3f,   0.0f, 1.0f,  // Top-left
     10.0f,  0.0f,  10.0f,   0.3f, 0.3f, 0.3f,   1.0f, 1.0f,  // Top-right
     10.0f,  0.0f, -10.0f,   0.3f, 0.3f, 0.3f,   1.0f, 0.0f   // Bottom-right
};

// FIXED Player cube vertex data (MUCH brighter blue color for visibility)
static float playerVertexData[] = {
    // Position            // Color              // Texture coords
    // Front face
    -0.8f, -0.8f,  0.8f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // Bottom-left
     0.8f, -0.8f,  0.8f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,  // Bottom-right
    -0.8f,  0.8f,  0.8f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,  // Top-left
     0.8f,  0.8f,  0.8f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,  // Top-right
    -0.8f,  0.8f,  0.8f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,  // Top-left
     0.8f, -0.8f,  0.8f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,  // Bottom-right

    // Back face
    -0.8f, -0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,  // Bottom-right
     0.7f, -0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // Bottom-left
    -0.7f,  0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,  // Top-right
     0.7f,  0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,  // Top-left
    -0.7f,  0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,  // Top-right
     0.7f, -0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // Bottom-left

    // Left face
    -0.8f, -0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // Bottom-left
    -0.8f, -0.7f,  0.7f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,  // Bottom-right
    -0.8f,  0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,  // Top-left
    -0.8f,  0.7f,  0.7f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,  // Top-right
    -0.8f,  0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,  // Top-left
    -0.7f, -0.7f,  0.7f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,  // Bottom-right

    // Right face
     0.7f, -0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,  // Bottom-right
     0.7f, -0.7f,  0.7f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f,  0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,  // Top-right
     0.7f,  0.7f,  0.7f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,  // Top-left
     0.7f,  0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,  // Top-right
     0.7f, -0.7f,  0.7f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // Bottom-left

    // Top face
    -0.7f,  0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,  // Top-left
     0.7f,  0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,  // Top-right
    -0.7f,  0.7f,  0.7f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f,  0.7f,  0.7f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,  // Bottom-right
    -0.7f,  0.7f,  0.7f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f,  0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,  // Top-right

    // Bottom face
    -0.7f, -0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f, -0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,  // Bottom-right
    -0.7f, -0.7f,  0.7f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,  // Top-left
     0.7f, -0.7f,  0.7f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,  // Top-right
    -0.7f, -0.7f,  0.7f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,  // Top-left
     0.7f, -0.7f, -0.7f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f   // Bottom-right
};

// FIXED Collectible vertex data - bright yellow cube with texture coordinates
static float collectibleVertexData[] = {
    // Front face
    -0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
    -0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
     0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right
     0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
    -0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left

    // Back face
    -0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
    -0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right
     0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
     0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
    -0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right

    // Left face
    -0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
    -0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
    -0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
    -0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right
    -0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
    -0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left

    // Right face
     0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
     0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right
     0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
     0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left

    // Top face
    -0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right
    -0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
     0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
    -0.6f,  0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
     0.6f,  0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right

    // Bottom face
    -0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
     0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
    -0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right
    -0.6f, -0.6f,  0.6f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.6f, -0.6f, -0.6f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f   // Bottom-right
};

// NPC vertex data arrays with different colors for each NPC
// Red NPC vertex data (for NPC 1)
static float npcVertexData1[] = {
    // Position            // Color              // Texture coords
    // Front face (bright red color)
    -0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
    -0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,  // Top-right
    -0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right

    // Back face
    -0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
     0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
    -0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,  // Top-right
     0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,  // Top-left
    -0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,  // Top-right
     0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left

    // Left face
    -0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
    -0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
    -0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,  // Top-left
    -0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,  // Top-right
    -0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,  // Top-left
    -0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right

    // Right face
     0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
     0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,  // Top-right
     0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,  // Top-right
     0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left

    // Top face
    -0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,  // Top-right
    -0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
    -0.7f,  0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f,  0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,  // Top-right

    // Bottom face
    -0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
    -0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,  // Top-right
    -0.7f, -0.7f,  0.7f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.7f, -0.7f, -0.7f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f   // Bottom-right
};

// Green NPC vertex data (for NPC 2)
static float npcVertexData2[] = {
    // Position            // Color              // Texture coords
    // Front face (bright green color)
    -0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
    -0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right
    -0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right

    // Back face
    -0.7f, -0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
     0.7f, -0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
    -0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right
     0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
    -0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right
     0.7f, -0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left

    // Left face
    -0.7f, -0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
    -0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
    -0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
    -0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right
    -0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
    -0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right

    // Right face
     0.7f, -0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
     0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right
     0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right
     0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left

    // Top face
    -0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right
    -0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
    -0.7f,  0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f,  0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right

    // Bottom face
    -0.7f, -0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f, -0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right
    -0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,  // Top-right
    -0.7f, -0.7f,  0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top-left
     0.7f, -0.7f, -0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f   // Bottom-right
};

// Blue NPC vertex data (for NPC 3)
static float npcVertexData3[] = {
    // Position            // Color              // Texture coords
    // Front face (bright blue color)
    -0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 0.0f,  // Bottom-right
    -0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 1.0f,  // Top-left
     0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
    -0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 1.0f,  // Top-left
     0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 0.0f,  // Bottom-right

    // Back face
    -0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 0.0f,  // Bottom-right
     0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left
    -0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
     0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 1.0f,  // Top-left
    -0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
     0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left

    // Left face
    -0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left
    -0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 0.0f,  // Bottom-right
    -0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 1.0f,  // Top-left
    -0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
    -0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 1.0f,  // Top-left
    -0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 0.0f,  // Bottom-right

    // Right face
     0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 0.0f,  // Bottom-right
     0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
     0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 1.0f,  // Top-left
     0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
     0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left

    // Top face
    -0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 1.0f,  // Top-left
     0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
    -0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 0.0f,  // Bottom-right
    -0.7f,  0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f,  0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 1.0f,  // Top-right

    // Bottom face
    -0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left
     0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 0.0f,  // Bottom-right
    -0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 1.0f,  // Top-left
     0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
    -0.7f, -0.7f,  0.7f,   0.0f, 0.5f, 1.0f,   0.0f, 1.0f,  // Top-left
     0.7f, -0.7f, -0.7f,   0.0f, 0.5f, 1.0f,   1.0f, 0.0f   // Bottom-right
};

//House vertex data - brown walls with texture coordinates
static float houseWallsVertexData[] = {
    // Front wall (with door hole)
    // Left part of front wall
    -3.0f,  0.0f,  3.0f,   0.6f, 0.4f, 0.2f,   0.0f, 0.0f,  // Bottom-left
    -3.0f,  2.0f,  3.0f,   0.6f, 0.4f, 0.2f,   0.0f, 1.0f,  // Top-left
    -1.0f,  0.0f,  3.0f,   0.6f, 0.4f, 0.2f,   0.5f, 0.0f,  // Bottom-right
    -1.0f,  2.0f,  3.0f,   0.6f, 0.4f, 0.2f,   0.5f, 1.0f,  // Top-right
    -1.0f,  0.0f,  3.0f,   0.6f, 0.4f, 0.2f,   0.5f, 0.0f,  // Bottom-right
    -3.0f,  2.0f,  3.0f,   0.6f, 0.4f, 0.2f,   0.0f, 1.0f,  // Top-left

    // Right part of front wall
    1.0f,   0.0f,  3.0f,   0.6f, 0.4f, 0.2f,   0.5f, 0.0f,  // Bottom-left
    1.0f,   2.0f,  3.0f,   0.6f, 0.4f, 0.2f,   0.5f, 1.0f,  // Top-left
    3.0f,   0.0f,  3.0f,   0.6f, 0.4f, 0.2f,   1.0f, 0.0f,  // Bottom-right
    3.0f,   2.0f,  3.0f,   0.6f, 0.4f, 0.2f,   1.0f, 1.0f,  // Top-right
    3.0f,   0.0f,  3.0f,   0.6f, 0.4f, 0.2f,   1.0f, 0.0f,  // Bottom-right
    1.0f,   2.0f,  3.0f,   0.6f, 0.4f, 0.2f,   0.5f, 1.0f,  // Top-left

    // Top part of front wall
    -1.0f,  2.0f,  3.0f,   0.6f, 0.4f, 0.2f,   0.0f, 0.0f,  // Bottom-left
    -1.0f,  3.0f,  3.0f,   0.6f, 0.4f, 0.2f,   0.0f, 0.5f,  // Top-left
    1.0f,   2.0f,  3.0f,   0.6f, 0.4f, 0.2f,   0.5f, 0.0f,  // Bottom-right
    1.0f,   3.0f,  3.0f,   0.6f, 0.4f, 0.2f,   0.5f, 0.5f,  // Top-right
    1.0f,   2.0f,  3.0f,   0.6f, 0.4f, 0.2f,   0.5f, 0.0f,  // Bottom-right
    -1.0f,  3.0f,  3.0f,   0.6f, 0.4f, 0.2f,   0.0f, 0.5f,  // Top-left

    // Back wall
    -3.0f,  0.0f, -3.0f,   0.6f, 0.4f, 0.2f,   0.0f, 0.0f,  // Bottom-left
    -3.0f,  3.0f, -3.0f,   0.6f, 0.4f, 0.2f,   0.0f, 1.0f,  // Top-left
    3.0f,   0.0f, -3.0f,   0.6f, 0.4f, 0.2f,   1.0f, 0.0f,  // Bottom-right
    3.0f,   3.0f, -3.0f,   0.6f, 0.4f, 0.2f,   1.0f, 1.0f,  // Top-right
    3.0f,   0.0f, -3.0f,   0.6f, 0.4f, 0.2f,   1.0f, 0.0f,  // Bottom-right
    -3.0f,  3.0f, -3.0f,   0.6f, 0.4f, 0.2f,   0.0f, 1.0f,  // Top-left

    // Left wall
    -3.0f,  0.0f, -3.0f,   0.6f, 0.4f, 0.2f,   0.0f, 0.0f,  // Bottom-back
    -3.0f,  3.0f, -3.0f,   0.6f, 0.4f, 0.2f,   0.0f, 1.0f,  // Top-back
    -3.0f,  0.0f,  3.0f,   0.6f, 0.4f, 0.2f,   1.0f, 0.0f,  // Bottom-front
    -3.0f,  3.0f,  3.0f,   0.6f, 0.4f, 0.2f,   1.0f, 1.0f,  // Top-front
    -3.0f,  0.0f,  3.0f,   0.6f, 0.4f, 0.2f,   1.0f, 0.0f,  // Bottom-front
    -3.0f,  3.0f, -3.0f,   0.6f, 0.4f, 0.2f,   0.0f, 1.0f,  // Top-back

    // Right wall
    3.0f,   0.0f, -3.0f,   0.6f, 0.4f, 0.2f,   0.0f, 0.0f,  // Bottom-back
    3.0f,   3.0f, -3.0f,   0.6f, 0.4f, 0.2f,   0.0f, 1.0f,  // Top-back
    3.0f,   0.0f,  3.0f,   0.6f, 0.4f, 0.2f,   1.0f, 0.0f,  // Bottom-front
    3.0f,   3.0f,  3.0f,   0.6f, 0.4f, 0.2f,   1.0f, 1.0f,  // Top-front
    3.0f,   0.0f,  3.0f,   0.6f, 0.4f, 0.2f,   1.0f, 0.0f,  // Bottom-front
    3.0f,   3.0f, -3.0f,   0.6f, 0.4f, 0.2f,   0.0f, 1.0f   // Top-back
};

// House door vertex data - dark brown (closed position) with texture coordinates
static float houseDoorVertexData[] = {
    // Door (slightly inset)
    -1.0f,  0.0f,  2.9f,   0.4f, 0.2f, 0.1f,   0.0f, 0.0f,  // Bottom-left
    -1.0f,  2.0f,  2.9f,   0.4f, 0.2f, 0.1f,   0.0f, 1.0f,  // Top-left
    1.0f,   0.0f,  2.9f,   0.4f, 0.2f, 0.1f,   1.0f, 0.0f,  // Bottom-right
    1.0f,   2.0f,  2.9f,   0.4f, 0.2f, 0.1f,   1.0f, 1.0f,  // Top-right
    1.0f,   0.0f,  2.9f,   0.4f, 0.2f, 0.1f,   1.0f, 0.0f,  // Bottom-right
    -1.0f,  2.0f,  2.9f,   0.4f, 0.2f, 0.1f,   0.0f, 1.0f   // Top-left
};

// House door vertex data - dark brown (open position - rotated 90 degrees) with texture coordinates
static float houseDoorOpenVertexData[] = {
    // Door (rotated to open position) - with a different color to make it obvious when open
    -1.0f,  0.0f,  2.9f,   0.9f, 0.5f, 0.2f,   0.0f, 0.0f,  // Bottom-left (hinge point) - brighter color
    -1.0f,  2.0f,  2.9f,   0.9f, 0.5f, 0.2f,   0.0f, 1.0f,  // Top-left (hinge point) - brighter color
    -1.0f,  0.0f,  0.9f,   0.9f, 0.5f, 0.2f,   1.0f, 0.0f,  // Bottom-right (new position) - brighter color
    -1.0f,  2.0f,  0.9f,   0.9f, 0.5f, 0.2f,   1.0f, 1.0f,  // Top-right (new position) - brighter color
    -1.0f,  0.0f,  0.9f,   0.9f, 0.5f, 0.2f,   1.0f, 0.0f,  // Bottom-right (new position) - brighter color
    -1.0f,  2.0f,  2.9f,   0.9f, 0.5f, 0.2f,   0.0f, 1.0f   // Top-left (hinge point) - brighter color
};

// House roof vertex data - red with texture coordinates
static float houseRoofVertexData[] = {
    // Front triangle
    -3.5f,  3.0f,  3.5f,   0.8f, 0.2f, 0.2f,   0.0f, 0.0f,  // Bottom-left
    3.5f,   3.0f,  3.5f,   0.8f, 0.2f, 0.2f,   1.0f, 0.0f,  // Bottom-right
    0.0f,   5.0f,  0.0f,   0.8f, 0.2f, 0.2f,   0.5f, 1.0f,  // Top

    // Back triangle
    -3.5f,  3.0f, -3.5f,   0.8f, 0.2f, 0.2f,   0.0f, 0.0f,  // Bottom-left
    3.5f,   3.0f, -3.5f,   0.8f, 0.2f, 0.2f,   1.0f, 0.0f,  // Bottom-right
    0.0f,   5.0f,  0.0f,   0.8f, 0.2f, 0.2f,   0.5f, 1.0f,  // Top

    // Left triangle
    -3.5f,  3.0f, -3.5f,   0.8f, 0.2f, 0.2f,   0.0f, 0.0f,  // Bottom-back
    -3.5f,  3.0f,  3.5f,   0.8f, 0.2f, 0.2f,   1.0f, 0.0f,  // Bottom-front
    0.0f,   5.0f,  0.0f,   0.8f, 0.2f, 0.2f,   0.5f, 1.0f,  // Top

    // Right triangle
    3.5f,   3.0f, -3.5f,   0.8f, 0.2f, 0.2f,   0.0f, 0.0f,  // Bottom-back
    3.5f,   3.0f,  3.5f,   0.8f, 0.2f, 0.2f,   1.0f, 0.0f,  // Bottom-front
    0.0f,   5.0f,  0.0f,   0.8f, 0.2f, 0.2f,   0.5f, 1.0f   // Top
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
    
    // If heightmap is enabled, place player on top of terrain
    if (mUseHeightMap && mHeightMap.getVertices().size() > 0) {
        // Get height at current position using the enhanced barycentric coordinate method
        float terrainHeight = mHeightMap.getHeightAt(mPlayerPosition.x(), mPlayerPosition.z());
        
        // Add a small offset to keep player visible above the terrain
        mPlayerPosition.setY(terrainHeight + 1.0f);
        
        qDebug() << "Positioned player on terrain at height:" << terrainHeight;
    } else {
        // Keep player on ground if no heightmap
        mPlayerPosition.setY(0.0f);
    }
    
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
    
    // If heightmap is enabled, place player on top of terrain
    if (mUseHeightMap && mHeightMap.getVertices().size() > 0) {
        // Get height at current position using the enhanced barycentric coordinate method
        float terrainHeight = mHeightMap.getHeightAt(mPlayerPosition.x(), mPlayerPosition.z());
        
        // Add a small offset to keep player visible above the terrain
        mPlayerPosition.setY(terrainHeight + 1.0f);
        
        qDebug() << "Positioned player on terrain at height:" << terrainHeight;
    } else {
        // Keep player on ground if no heightmap
        mPlayerPosition.setY(0.0f);
    }
    
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
    
    // If heightmap is enabled, place player on top of terrain
    if (mUseHeightMap && mHeightMap.getVertices().size() > 0) {
        // Get height at current position using the enhanced barycentric coordinate method
        float terrainHeight = mHeightMap.getHeightAt(mPlayerPosition.x(), mPlayerPosition.z());
        
        // Add a small offset to keep player visible above the terrain
        mPlayerPosition.setY(terrainHeight + 1.0f);
        
        qDebug() << "Positioned player on terrain at height:" << terrainHeight;
    } else {
        // Force Y to be 0 to keep player on ground if no heightmap
        mPlayerPosition.setY(0.0f);
    }

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

    // Load and create texture resources
    createTextureImage();
    createTextureImageView();
    createTextureSampler();

    // Create a default texture (white 2x2 texture) if no texture is loaded
    // createDefaultTexture(logicalDevice);
    
    // Load the crate cube model
    loadCrateCubeModel();
    
    // Load NPC 3D model for later use
    loadNPCModel();

   initializeHeightMap();


    // Create lighting uniform buffer
    const VkDeviceSize lightingBufferSize = sizeof(LightingInfo);
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    // Create staging buffer for lighting data
    VkBufferCreateInfo stagingBufferInfo = {};
    stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferInfo.size = lightingBufferSize;
    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    err = mDeviceFunctions->vkCreateBuffer(logicalDevice, &stagingBufferInfo, nullptr, &stagingBuffer);
    if (err != VK_SUCCESS)
        qFatal("Failed to create lighting staging buffer: %d", err);
    
    VkMemoryRequirements stagingMemReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(logicalDevice, stagingBuffer, &stagingMemReq);
    
    VkMemoryAllocateInfo stagingAllocInfo = {};
    stagingAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    stagingAllocInfo.allocationSize = stagingMemReq.size;
    // Get memory properties
    VkPhysicalDeviceMemoryProperties memProperties;
    mWindow->vulkanInstance()->functions()->vkGetPhysicalDeviceMemoryProperties(mWindow->physicalDevice(), &memProperties);
    
    stagingAllocInfo.memoryTypeIndex = getMemoryTypeIndex(memProperties, 
                                                         stagingMemReq.memoryTypeBits, 
                                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    err = mDeviceFunctions->vkAllocateMemory(logicalDevice, &stagingAllocInfo, nullptr, &stagingBufferMemory);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate lighting staging buffer memory: %d", err);
    
    err = mDeviceFunctions->vkBindBufferMemory(logicalDevice, stagingBuffer, stagingBufferMemory, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind lighting staging buffer memory: %d", err);
    
    // Create actual lighting buffer
    VkBufferCreateInfo lightingBufferInfo = {};
    lightingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    lightingBufferInfo.size = lightingBufferSize;
    lightingBufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    lightingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    err = mDeviceFunctions->vkCreateBuffer(logicalDevice, &lightingBufferInfo, nullptr, &mLightingBuffer);
    if (err != VK_SUCCESS)
        qFatal("Failed to create lighting buffer: %d", err);
    
    VkMemoryRequirements lightingMemReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(logicalDevice, mLightingBuffer, &lightingMemReq);
    
    // Reuse memProperties from above
    mWindow->vulkanInstance()->functions()->vkGetPhysicalDeviceMemoryProperties(mWindow->physicalDevice(), &memProperties);
    
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = lightingMemReq.size;
    allocInfo.memoryTypeIndex = getMemoryTypeIndex(memProperties, 
                                                 lightingMemReq.memoryTypeBits, 
                                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    
    err = mDeviceFunctions->vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &mLightingBufferMemory);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate lighting buffer memory: %d", err);
    
    err = mDeviceFunctions->vkBindBufferMemory(logicalDevice, mLightingBuffer, mLightingBufferMemory, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind lighting buffer memory: %d", err);
    
    // Copy lighting data to staging buffer
    void* data;
    err = mDeviceFunctions->vkMapMemory(logicalDevice, stagingBufferMemory, 0, lightingBufferSize, 0, &data);
    if (err != VK_SUCCESS)
        qFatal("Failed to map lighting staging buffer memory: %d", err);
    
    // Initialize lighting data
    mLightingData.lightPosition[0] = 2.0f;
    mLightingData.lightPosition[1] = 2.0f;
    mLightingData.lightPosition[2] = 2.0f;
    mLightingData.padding1 = 0.0f;
    mLightingData.viewPosition[0] = 0.0f;
    mLightingData.viewPosition[1] = 0.0f;
    mLightingData.viewPosition[2] = 5.0f;
    mLightingData.padding2 = 0.0f;
    mLightingData.lightColor[0] = 1.0f;
    mLightingData.lightColor[1] = 1.0f;
    mLightingData.lightColor[2] = 1.0f;
    mLightingData.ambientStrength = 0.2f;
    mLightingData.specularStrength = 0.5f;
    mLightingData.shininess = 32.0f;
    mLightingData.padding3[0] = 0.0f;
    mLightingData.padding3[1] = 0.0f;
    
    memcpy(data, &mLightingData, sizeof(LightingInfo));
    mDeviceFunctions->vkUnmapMemory(logicalDevice, stagingBufferMemory);
    
    // Copy from staging buffer to device local buffer
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    
    VkBufferCopy copyRegion = {};
    copyRegion.size = lightingBufferSize;
    mDeviceFunctions->vkCmdCopyBuffer(commandBuffer, stagingBuffer, mLightingBuffer, 1, &copyRegion);
    
    endSingleTimeCommands(commandBuffer);
    
    // Clean up staging buffer
    mDeviceFunctions->vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    mDeviceFunctions->vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
    
    // Set up lighting buffer info for descriptor sets
    for (int i = 0; i < concurrentFrameCount; ++i) {
        mLightingBufferInfo[i].buffer = mLightingBuffer;
        mLightingBufferInfo[i].offset = 0;
        mLightingBufferInfo[i].range = sizeof(LightingInfo);
    }

    /********************************* Vertex layout: *********************************/
    //The size of each vertex to be passed to the shader
    VkVertexInputBindingDescription vertexBindingDesc = {
        0, // binding - has to match that in VkVertexInputAttributeDescription and startNextFrame()s m_devFuncs->vkCmdBindVertexBuffers
        8 * sizeof(float), // stride account for X, Y, Z, R, G, B, U, V (positions, colors, texture coords)
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
        },
        { // texture coordinates
            2, // location has to correspond to the layout(location = x) in the shader
            0, // binding
            VK_FORMAT_R32G32_SFLOAT,
            6 * sizeof(float) // offset to account for X, Y, Z, R, G, B
        }
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;
    vertexInputInfo.flags = 0;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDesc;
    vertexInputInfo.vertexAttributeDescriptionCount = 3; // Now three attributes instead of two
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttrDesc;

    // Set up descriptor pool for all objects (ground, player, collectibles, and 3 separate NPCs)
    VkDescriptorPoolSize descPoolSizes[2] = {};
    
    // First type: Uniform buffers
    descPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descPoolSizes[0].descriptorCount = 20 * concurrentFrameCount; // 2 for each buffer per frame (MVP + lighting)
    
    // Second type: Combined image samplers
    descPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descPoolSizes[1].descriptorCount = 10 * concurrentFrameCount; // 1 for each object that needs textures per frame
    
    VkDescriptorPoolCreateInfo descPoolInfo = {};
    descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolInfo.maxSets = 20 * concurrentFrameCount; // Total number of descriptor sets
    descPoolInfo.poolSizeCount = 2; // Two types
    descPoolInfo.pPoolSizes = descPoolSizes;
    
    // Destroy old pool if it exists
    if (mDescriptorPool != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyDescriptorPool(mWindow->device(), mDescriptorPool, nullptr);
        mDescriptorPool = VK_NULL_HANDLE;
    }
    
    // Use existing err variable without redeclaring it
    err = mDeviceFunctions->vkCreateDescriptorPool(logicalDevice, &descPoolInfo, nullptr, &mDescriptorPool);
    if (err != VK_SUCCESS)
        qFatal("Failed to create descriptor pool: %d", err);

    /********************************* Descriptor Set Layout: Uniform buffer and texture sampler *********************************/
    // Create three bindings: one for uniform buffer, one for texture sampler, and one for lighting
    VkDescriptorSetLayoutBinding layoutBindings[3] = {};
    
    // Binding 0: Uniform buffer (MVP matrix)
    layoutBindings[0].binding = 0;
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBindings[0].descriptorCount = 1;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBindings[0].pImmutableSamplers = nullptr;
    
    // Binding 1: Texture sampler (for fragment shader)
    layoutBindings[1].binding = 1;
    layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layoutBindings[1].descriptorCount = 1;
    layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    layoutBindings[1].pImmutableSamplers = nullptr;
    
    // Binding 2: Lighting uniform buffer (for fragment shader)
    layoutBindings[2].binding = 2;
    layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBindings[2].descriptorCount = 1;
    layoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    layoutBindings[2].pImmutableSamplers = nullptr;
    
    VkDescriptorSetLayoutCreateInfo descLayoutInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        nullptr,
        0,
        3,  // Three bindings now
        layoutBindings
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

        // Create texture sampler descriptor for all descriptor sets
        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = mTextureImageView;
        imageInfo.sampler = mTextureSampler;

        // We need three write descriptor sets: one for uniform buffer, one for texture sampler, one for lighting
        VkWriteDescriptorSet writeDescSets[3] = {};
        
        // First write: Uniform buffer (MVP matrix)
        writeDescSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescSets[0].dstSet = mDescriptorSet[i];
        writeDescSets[0].dstBinding = 0;
        writeDescSets[0].descriptorCount = 1;
        writeDescSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescSets[0].pBufferInfo = &mUniformBufferInfo[i];
        
        // Second write: Texture sampler
        writeDescSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescSets[1].dstSet = mDescriptorSet[i];
        writeDescSets[1].dstBinding = 1;
        writeDescSets[1].descriptorCount = 1;
        writeDescSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescSets[1].pImageInfo = &imageInfo;
        
        // Third write: Lighting uniform buffer
        writeDescSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescSets[2].dstSet = mDescriptorSet[i];
        writeDescSets[2].dstBinding = 2;
        writeDescSets[2].descriptorCount = 1;
        writeDescSets[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescSets[2].pBufferInfo = &mLightingBufferInfo[i];
        
        // Update all three descriptor writes
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 3, writeDescSets, 0, nullptr);
        
        // Player descriptor set 
        err = mDeviceFunctions->vkAllocateDescriptorSets(logicalDevice, &descSetAllocInfo, &mPlayerDescriptorSet[i]);
        if (err != VK_SUCCESS)
            qFatal("Failed to allocate player descriptor set: %d", err);
            
        // Update player descriptor set with uniform buffer, texture, and lighting
        writeDescSets[0].dstSet = mPlayerDescriptorSet[i];
        writeDescSets[0].pBufferInfo = &mPlayerUniformBufferInfo[i];
        writeDescSets[1].dstSet = mPlayerDescriptorSet[i];
        writeDescSets[2].dstSet = mPlayerDescriptorSet[i];
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 3, writeDescSets, 0, nullptr);
        
        // Collectible descriptor set 
        err = mDeviceFunctions->vkAllocateDescriptorSets(logicalDevice, &descSetAllocInfo, &mCollectibleDescriptorSet[i]);
        if (err != VK_SUCCESS)
            qFatal("Failed to allocate collectible descriptor set: %d", err);
            
        // Update collectible descriptor set with uniform, texture, and lighting
        writeDescSets[0].dstSet = mCollectibleDescriptorSet[i];
        writeDescSets[0].pBufferInfo = &mCollectibleUniformBufferInfo[i];
        writeDescSets[1].dstSet = mCollectibleDescriptorSet[i];
        writeDescSets[2].dstSet = mCollectibleDescriptorSet[i];
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 3, writeDescSets, 0, nullptr);
        
        // NPC 1 (Red) descriptor set 
        err = mDeviceFunctions->vkAllocateDescriptorSets(logicalDevice, &descSetAllocInfo, &mNPCDescriptorSet1[i]);
        if (err != VK_SUCCESS)
            qFatal("Failed to allocate NPC1 descriptor set: %d", err);
            
        // Update NPC1 descriptor set with uniform, texture, and lighting
        writeDescSets[0].dstSet = mNPCDescriptorSet1[i];
        writeDescSets[0].pBufferInfo = &mNPCUniformBufferInfo1[i];
        writeDescSets[1].dstSet = mNPCDescriptorSet1[i]; 
        writeDescSets[2].dstSet = mNPCDescriptorSet1[i];
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 3, writeDescSets, 0, nullptr);
        
        // NPC 2 (Green) descriptor set 
        err = mDeviceFunctions->vkAllocateDescriptorSets(logicalDevice, &descSetAllocInfo, &mNPCDescriptorSet2[i]);
        if (err != VK_SUCCESS)
            qFatal("Failed to allocate NPC2 descriptor set: %d", err);
            
        // Update NPC2 descriptor set with uniform, texture, and lighting
        writeDescSets[0].dstSet = mNPCDescriptorSet2[i];
        writeDescSets[0].pBufferInfo = &mNPCUniformBufferInfo2[i];
        writeDescSets[1].dstSet = mNPCDescriptorSet2[i];
        writeDescSets[2].dstSet = mNPCDescriptorSet2[i];
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 3, writeDescSets, 0, nullptr);
        
        // NPC 3 (Blue) descriptor set 
        err = mDeviceFunctions->vkAllocateDescriptorSets(logicalDevice, &descSetAllocInfo, &mNPCDescriptorSet3[i]);
        if (err != VK_SUCCESS)
            qFatal("Failed to allocate NPC3 descriptor set: %d", err);
            
        // Update NPC3 descriptor set with uniform, texture, and lighting
        writeDescSets[0].dstSet = mNPCDescriptorSet3[i];
        writeDescSets[0].pBufferInfo = &mNPCUniformBufferInfo3[i];
        writeDescSets[1].dstSet = mNPCDescriptorSet3[i];
        writeDescSets[2].dstSet = mNPCDescriptorSet3[i];
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 3, writeDescSets, 0, nullptr);
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
// Create another pipeline for objects without textures (ground plane)
// Use the same pipeline layout and renderpass
VkPipelineShaderStageCreateInfo noTextureStages[2] = {};
noTextureStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
noTextureStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
mNoTextureVertShader = createShader(QStringLiteral(":/noTexture_vert.spv"));
if (mNoTextureVertShader == VK_NULL_HANDLE) {
    qWarning() << "Failed to load no-texture vertex shader, falling back to regular rendering";
    goto skip_no_texture_pipeline;
}
noTextureStages[0].module = mNoTextureVertShader;
noTextureStages[0].pName = "main";

noTextureStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
noTextureStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
mNoTextureFragShader = createShader(QStringLiteral(":/noTexture_frag.spv"));
if (mNoTextureFragShader == VK_NULL_HANDLE) {
    qWarning() << "Failed to load no-texture fragment shader, falling back to regular rendering";
    if (mNoTextureVertShader != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyShaderModule(logicalDevice, mNoTextureVertShader, nullptr);
        mNoTextureVertShader = VK_NULL_HANDLE;
    }
    goto skip_no_texture_pipeline;
}
noTextureStages[1].module = mNoTextureFragShader;
noTextureStages[1].pName = "main";

VkGraphicsPipelineCreateInfo noTexturePipelineInfo = pipelineInfo; // Copy settings from the textured pipeline
noTexturePipelineInfo.stageCount = 2;
noTexturePipelineInfo.pStages = noTextureStages;

err = mDeviceFunctions->vkCreateGraphicsPipelines(logicalDevice, mPipelineCache, 1, &noTexturePipelineInfo, nullptr, &mNoTexturePipeline);
if (err != VK_SUCCESS) {
    qWarning() << "Failed to create no-texture graphics pipeline:" << err << ", falling back to regular rendering";
    if (mNoTextureVertShader != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyShaderModule(logicalDevice, mNoTextureVertShader, nullptr);
        mNoTextureVertShader = VK_NULL_HANDLE;
    }
    if (mNoTextureFragShader != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyShaderModule(logicalDevice, mNoTextureFragShader, nullptr);
        mNoTextureFragShader = VK_NULL_HANDLE;
    }
    goto skip_no_texture_pipeline;
}

if (mNoTextureVertShader) {
    mDeviceFunctions->vkDestroyShaderModule(logicalDevice, mNoTextureVertShader, nullptr);
    mNoTextureVertShader = VK_NULL_HANDLE;
}
if (mNoTextureFragShader) {
    mDeviceFunctions->vkDestroyShaderModule(logicalDevice, mNoTextureFragShader, nullptr);
    mNoTextureFragShader = VK_NULL_HANDLE;
}

skip_no_texture_pipeline:
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

        // Create texture sampler descriptor
        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = mTextureImageView;
        imageInfo.sampler = mTextureSampler;

        // Three write descriptor sets: one for uniform buffer, one for texture sampler, one for lighting
        VkWriteDescriptorSet writeDescSets[3] = {};
        
        // First write: Uniform buffer
        writeDescSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescSets[0].dstSet = mHouseDescriptorSet[i];
        writeDescSets[0].dstBinding = 0;
        writeDescSets[0].descriptorCount = 1;
        writeDescSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescSets[0].pBufferInfo = &mHouseUniformBufferInfo[i];
        
        // Second write: Texture sampler
        writeDescSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescSets[1].dstSet = mHouseDescriptorSet[i];
        writeDescSets[1].dstBinding = 1;
        writeDescSets[1].descriptorCount = 1;
        writeDescSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescSets[1].pImageInfo = &imageInfo;
        
        // Third write: Lighting uniform buffer
        writeDescSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescSets[2].dstSet = mHouseDescriptorSet[i];
        writeDescSets[2].dstBinding = 2;
        writeDescSets[2].descriptorCount = 1;
        writeDescSets[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescSets[2].pBufferInfo = &mLightingBufferInfo[i];
        
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 3, writeDescSets, 0, nullptr);
    }
    
    // Create descriptor sets for the indoor scene
    for (int i = 0; i < concurrentFrameCount; ++i) {
        err = mDeviceFunctions->vkAllocateDescriptorSets(logicalDevice, &houseDescSetAllocInfo, &mIndoorDescriptorSet[i]);
        if (err != VK_SUCCESS)
            qFatal("Failed to allocate indoor descriptor set: %d", err);

        // Create texture sampler descriptor
        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = mTextureImageView;
        imageInfo.sampler = mTextureSampler;

        // Three descriptor writes: one for uniform buffer, one for texture sampler, one for lighting
        VkWriteDescriptorSet descriptorWrites[3] = {};
        
        // Uniform buffer descriptor
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = mIndoorDescriptorSet[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].pBufferInfo = &mIndoorUniformBufferInfo[i];
        
        // Texture sampler descriptor
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = mIndoorDescriptorSet[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].pImageInfo = &imageInfo;
        
        // Lighting uniform buffer descriptor
        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = mIndoorDescriptorSet[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[2].pBufferInfo = &mLightingBufferInfo[i];
        
        // Update the descriptor sets with all three writes
        mDeviceFunctions->vkUpdateDescriptorSets(logicalDevice, 3, descriptorWrites, 0, nullptr);
    }

    qDebug("\n ***************************** initResources finished ******************************************* \n");

    getVulkanHWInfo();


    
    // Initialize indoor scene resources
    createIndoorSceneResources();
    
    // Initialize default scene state
    mCurrentScene = 1; // Start in outdoor scene
    
    // Initialize lighting data
    mLightingData.lightPosition[0] = 2.0f;
    mLightingData.lightPosition[1] = 2.0f;
    mLightingData.lightPosition[2] = 2.0f;
    mLightingData.viewPosition[0] = 0.0f;
    mLightingData.viewPosition[1] = 0.0f;
    mLightingData.viewPosition[2] = 5.0f;
    mLightingData.lightColor[0] = 1.0f;
    mLightingData.lightColor[1] = 1.0f;
    mLightingData.lightColor[2] = 1.0f;
    mLightingData.ambientStrength = 0.2f;
    mLightingData.specularStrength = 0.5f;
    mLightingData.shininess = 32.0f;
    
    // Initialize resources that need to be regenerated on window resize
    initSwapChainResources();
}

void RenderWindow::createIndoorSceneResources()
{
    qDebug() << "Creating indoor scene resources...";
    VkDevice device = mWindow->device();
    
    // Indoor room vertices (simple cube room)
    static const float indoorWallsVertexData[] = {
        // Position            // Color (white walls with blue accent)  // Texture coords
        // Back wall (negative Z)
        -5.0f, 0.0f, -5.0f,    0.9f, 0.9f, 1.0f,   0.0f, 0.0f,  // Bottom-left
        5.0f, 0.0f, -5.0f,     0.9f, 0.9f, 1.0f,   1.0f, 0.0f,  // Bottom-right
        5.0f, 5.0f, -5.0f,     0.9f, 0.9f, 1.0f,   1.0f, 1.0f,  // Top-right
        -5.0f, 5.0f, -5.0f,    0.9f, 0.9f, 1.0f,   0.0f, 1.0f,  // Top-left
        
        // Front wall (positive Z) with door cutout
        -5.0f, 0.0f, 5.0f,     0.9f, 0.9f, 1.0f,   0.0f, 0.0f,  // Bottom-left
        -1.5f, 0.0f, 5.0f,     0.9f, 0.9f, 1.0f,   0.3f, 0.0f,  // Door left bottom
        -1.5f, 3.0f, 5.0f,     0.9f, 0.9f, 1.0f,   0.3f, 0.6f,  // Door left top
        -5.0f, 5.0f, 5.0f,     0.9f, 0.9f, 1.0f,   0.0f, 1.0f,  // Top-left
        
        // Front wall right of door
        1.5f, 0.0f, 5.0f,      0.9f, 0.9f, 1.0f,   0.7f, 0.0f,  // Door right bottom
        5.0f, 0.0f, 5.0f,      0.9f, 0.9f, 1.0f,   1.0f, 0.0f,  // Bottom-right
        5.0f, 5.0f, 5.0f,      0.9f, 0.9f, 1.0f,   1.0f, 1.0f,  // Top-right
        1.5f, 3.0f, 5.0f,      0.9f, 0.9f, 1.0f,   0.7f, 0.6f,  // Door right top
        
        // Front wall above door
        -1.5f, 3.0f, 5.0f,     0.9f, 0.9f, 1.0f,   0.3f, 0.6f,  // Door left top
        1.5f, 3.0f, 5.0f,      0.9f, 0.9f, 1.0f,   0.7f, 0.6f,  // Door right top
        1.5f, 5.0f, 5.0f,      0.9f, 0.9f, 1.0f,   0.7f, 1.0f,  // Top-right of door
        -1.5f, 5.0f, 5.0f,     0.9f, 0.9f, 1.0f,   0.3f, 1.0f,  // Top-left of door
        
        // Left wall (negative X)
        -5.0f, 0.0f, -5.0f,    0.8f, 0.8f, 1.0f,   0.0f, 0.0f,  // Back bottom
        -5.0f, 0.0f, 5.0f,     0.8f, 0.8f, 1.0f,   1.0f, 0.0f,  // Front bottom
        -5.0f, 5.0f, 5.0f,     0.8f, 0.8f, 1.0f,   1.0f, 1.0f,  // Front top
        -5.0f, 5.0f, -5.0f,    0.8f, 0.8f, 1.0f,   0.0f, 1.0f,  // Back top
        
        // Right wall (positive X)
        5.0f, 0.0f, -5.0f,     0.8f, 0.8f, 1.0f,   1.0f, 0.0f,  // Back bottom
        5.0f, 0.0f, 5.0f,      0.8f, 0.8f, 1.0f,   0.0f, 0.0f,  // Front bottom
        5.0f, 5.0f, 5.0f,      0.8f, 0.8f, 1.0f,   0.0f, 1.0f,  // Front top
        5.0f, 5.0f, -5.0f,     0.8f, 0.8f, 1.0f,   1.0f, 1.0f,  // Back top
        
        // Ceiling (positive Y)
        -5.0f, 5.0f, -5.0f,    0.7f, 0.7f, 1.0f,   0.0f, 0.0f,  // Back-left
        5.0f, 5.0f, -5.0f,     0.7f, 0.7f, 1.0f,   1.0f, 0.0f,  // Back-right
        5.0f, 5.0f, 5.0f,      0.7f, 0.7f, 1.0f,   1.0f, 1.0f,  // Front-right
        -5.0f, 5.0f, 5.0f,     0.7f, 0.7f, 1.0f,   0.0f, 1.0f,  // Front-left
    };
    
    // Exit door vertices
    static const float exitDoorVertexData[] = {
        // Position            // Color (wooden door)    // Texture coords
        -1.5f, 0.0f, 5.0f,     0.6f, 0.4f, 0.2f,   0.0f, 0.0f,  // Bottom-left
        1.5f, 0.0f, 5.0f,      0.6f, 0.4f, 0.2f,   1.0f, 0.0f,  // Bottom-right
        1.5f, 3.0f, 5.0f,      0.6f, 0.4f, 0.2f,   1.0f, 1.0f,  // Top-right
        -1.5f, 3.0f, 5.0f,     0.6f, 0.4f, 0.2f,   0.0f, 1.0f,  // Top-left
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
        
        // Create texture sampler descriptor info
        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = mTextureImageView;
        imageInfo.sampler = mTextureSampler;

        // Three descriptor writes: one for uniform buffer, one for texture sampler, one for lighting
        VkWriteDescriptorSet descriptorWrites[3] = {};
        
        // Uniform buffer descriptor
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = mIndoorDescriptorSet[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].pBufferInfo = &mIndoorUniformBufferInfo[i];
        
        // Texture sampler descriptor
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = mIndoorDescriptorSet[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].pImageInfo = &imageInfo;
        
        // Lighting uniform buffer descriptor
        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = mIndoorDescriptorSet[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[2].pBufferInfo = &mLightingBufferInfo[i];
        
        // Update the descriptor sets with all three writes
        mDeviceFunctions->vkUpdateDescriptorSets(device, 3, descriptorWrites, 0, nullptr);
    }
    
    qDebug() << "Initialized indoor scene resources successfully";
}

void RenderWindow::initializeHeightMap()
{
    // Create the heightmap from the image
    qDebug() << "Initializing HeightMap...";
    
    bool heightmapCreated = false;
    
    // Try direct loading with Qt Resource system
    QFile resourceFile(":/assets/heightmaps/Heightmap.jpg");
    if (resourceFile.exists()) {
        qDebug() << "Resource file exists in Qt resource system";
        mHeightMap.makeTerrain(":/assets/heightmaps/Heightmap.jpg");
        
        // Check if heightmap was created
        if (!mHeightMap.getVertices().empty() && !mHeightMap.getIndices().empty()) {
            qDebug() << "Heightmap created successfully from resource";
            heightmapCreated = true;
        } else {
            qDebug() << "Failed to create heightmap from resource file";
        }
    } else {
        qDebug() << "Resource file DOES NOT exist in Qt resource system";
        
        // Try to find the file in alternative locations
        QStringList possiblePaths = {
            "assets/heightmaps/Heightmap.jpg",
            "../assets/heightmaps/Heightmap.jpg",
            "../../assets/heightmaps/Heightmap.jpg",
            "../QtVulkanApp-main/assets/heightmaps/Heightmap.jpg",
            "C:/Users/romha/Downloads/QtVulkanApp-main/assets/heightmaps/Heightmap.jpg"
        };
        
        for (const QString& path : possiblePaths) {
            QFile file(path);
            if (file.exists()) {
                qDebug() << "Found heightmap at:" << path;
                mHeightMap.makeTerrain(path.toStdString());
                
                // Check if heightmap was created
                if (!mHeightMap.getVertices().empty() && !mHeightMap.getIndices().empty()) {
                    qDebug() << "Heightmap created successfully from file:" << path;
                    heightmapCreated = true;
                    break;
                } else {
                    qDebug() << "Failed to create heightmap from file:" << path;
                }
            }
        }
    }
    
    // If no heightmap created yet, create a procedural one
    if (!heightmapCreated) {
        qDebug() << "Creating procedural heightmap...";
        
        // Create a simple procedural heightmap (128x128 grayscale image)
        const int width = 128;
        const int height = 128;
        
        // Allocate memory for RGBA data (4 bytes per pixel)
        unsigned char* heightmapData = new unsigned char[width * height * 4];
        
        // Generate a simple hill/valley pattern
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // Create a sin wave pattern
                float dx = (x / (float)width - 0.5f) * 10.0f;
                float dy = (y / (float)height - 0.5f) * 10.0f;
                float dist = sqrt(dx*dx + dy*dy);
                
                // Combine multiple sin waves for more interesting terrain
                float heightValue = 
                    128.0f + 
                    64.0f * sin(dist) + 
                    32.0f * sin(dx*2) * sin(dy*2);
                
                // Clamp to 0-255
                unsigned char pixelValue = (unsigned char)std::max(0.0f, std::min(255.0f, heightValue));
                
                // Set RGBA values (using the same value for all channels)
                heightmapData[(y * width + x) * 4 + 0] = pixelValue; // R
                heightmapData[(y * width + x) * 4 + 1] = pixelValue; // G
                heightmapData[(y * width + x) * 4 + 2] = pixelValue; // B
                heightmapData[(y * width + x) * 4 + 3] = 255;        // A (fully opaque)
            }
        }
        
        qDebug() << "Created procedural heightmap data, trying to use it...";
        mHeightMap.makeTerrain(heightmapData, width, height);
        
        // Check if heightmap was created
        if (!mHeightMap.getVertices().empty() && !mHeightMap.getIndices().empty()) {
            qDebug() << "Procedural heightmap created successfully";
            heightmapCreated = true;
        } else {
            qDebug() << "Failed to create procedural heightmap";
        }
        
        // Clean up
        delete[] heightmapData;
    }
    
    // Check if the heightmap was loaded/created successfully
    if (!heightmapCreated) {
        qDebug() << "ERROR: HeightMap generation failed - no vertices or indices!";
        return;
    }
    
    const std::vector<ObjLoader::Vertex>& vertices = mHeightMap.getVertices();
    const std::vector<uint32_t>& indices = mHeightMap.getIndices();
    
    qDebug() << "HeightMap generated with" << vertices.size() << "vertices and" << indices.size() << "indices";
    
    // Create vertex buffer
    VkDeviceSize vertexBufferSize = sizeof(ObjLoader::Vertex) * vertices.size();
    
    // Create staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );
    
    // Map memory and copy vertices
    void* data;
    mDeviceFunctions->vkMapMemory(mWindow->device(), stagingBufferMemory, 0, vertexBufferSize, 0, &data);
    memcpy(data, vertices.data(), vertexBufferSize);
    mDeviceFunctions->vkUnmapMemory(mWindow->device(), stagingBufferMemory);
    
    // Create vertex buffer
    createBuffer(
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        mHeightMapVertexBuffer,
        mHeightMapVertexBufferMemory
    );
    
    // Copy staging buffer to vertex buffer
    copyBuffer(stagingBuffer, mHeightMapVertexBuffer, vertexBufferSize);
    
    // Clean up staging buffer
    mDeviceFunctions->vkDestroyBuffer(mWindow->device(), stagingBuffer, nullptr);
    mDeviceFunctions->vkFreeMemory(mWindow->device(), stagingBufferMemory, nullptr);
    
    // Create index buffer
    VkDeviceSize indexBufferSize = sizeof(uint32_t) * indices.size();
    
    // Create staging buffer for indices
    createBuffer(
        indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );
    
    // Map memory and copy indices
    mDeviceFunctions->vkMapMemory(mWindow->device(), stagingBufferMemory, 0, indexBufferSize, 0, &data);
    memcpy(data, indices.data(), indexBufferSize);
    mDeviceFunctions->vkUnmapMemory(mWindow->device(), stagingBufferMemory);
    
    // Create index buffer
    createBuffer(
        indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        mHeightMapIndexBuffer,
        mHeightMapIndexBufferMemory
    );
    
    // Copy staging buffer to index buffer
    copyBuffer(stagingBuffer, mHeightMapIndexBuffer, indexBufferSize);
    
    // Clean up staging buffer
    mDeviceFunctions->vkDestroyBuffer(mWindow->device(), stagingBuffer, nullptr);
    mDeviceFunctions->vkFreeMemory(mWindow->device(), stagingBufferMemory, nullptr);
    
    // Enable heightmap rendering
    mUseHeightMap = true;
    
    qDebug() << "HeightMap initialization complete!";
    adjustCollectibleHeights();
}

void RenderWindow::adjustCollectibleHeights()
{
    if (mUseHeightMap && mHeightMap.getVertices().size() > 0) {
        qDebug() << "Adjusting collectible heights based on heightmap...";
        for (int i = 0; i < mCollectibles.size(); i++) {
            // Get height at collectible position
            float terrainHeight = mHeightMap.getHeightAt(mCollectibles[i].position.x(), mCollectibles[i].position.z());
            
            // Place collectible above the terrain with an increased offset to make it clearly visible
            mCollectibles[i].position.setY(terrainHeight + 4.0f); // Large offset for better visibility
            
            qDebug() << "Placed collectible" << i << "at height:" << mCollectibles[i].position.y() 
                     << "over terrain height:" << terrainHeight;
        }
    } else {
        qDebug() << "WARNING: Cannot adjust collectible heights - heightmap condition not met";
    }
} 
void RenderWindow::drawOutdoorScene(VkDevice device, VkCommandBuffer cb, quint8* GPUmemPointer, VkResult& err)
{


   // Draw heightmap terrain
   if (mUseHeightMap && mHeightMapVertexBuffer != VK_NULL_HANDLE && mHeightMapIndexBuffer != VK_NULL_HANDLE) {
       qDebug() << "Drawing HeightMap terrain...";
       
       // Set up model-view-projection matrix for heightmap
       QMatrix4x4 groundMatrix;
       groundMatrix.setToIdentity();
       QMatrix4x4 groundMVP = mProjectionMatrix * mViewMatrix * groundMatrix;

       // Update uniform buffer for heightmap
       VkDeviceSize groundOffset = mUniformBufferInfo[mWindow->currentFrame()].offset;
       err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, groundOffset,
                                         UNIFORM_DATA_SIZE, 0, reinterpret_cast<void **>(&GPUmemPointer));
       memcpy(GPUmemPointer, groundMVP.constData(), 16 * sizeof(float));
       mDeviceFunctions->vkUnmapMemory(device, mBufferMemory);

       // Use no-texture pipeline for heightmap
       if (mNoTexturePipeline != VK_NULL_HANDLE) {
           mDeviceFunctions->vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mNoTexturePipeline);
       } else {
           mDeviceFunctions->vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
       }

       // Bind descriptor sets and vertex/index buffers
       mDeviceFunctions->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                               &mDescriptorSet[mWindow->currentFrame()], 0, nullptr);
       
       // Bind the heightmap vertex buffer
       VkDeviceSize heightMapVertexOffset = 0;
       mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mHeightMapVertexBuffer, &heightMapVertexOffset);
       
       // Bind the heightmap index buffer and draw using indices
       mDeviceFunctions->vkCmdBindIndexBuffer(cb, mHeightMapIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
       
       // Draw the heightmap using indices
       const std::vector<uint32_t>& indices = mHeightMap.getIndices();
       mDeviceFunctions->vkCmdDrawIndexed(cb, indices.size(), 1, 0, 0, 0);

       qDebug() << "Drew HeightMap terrain with" << indices.size() << "indices";
       
       // Switch back to regular pipeline for other objects
       mDeviceFunctions->vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
   } else {
       // Fallback to the old ground drawing if heightmap is not available
       QMatrix4x4 groundMatrix;
       groundMatrix.setToIdentity();
       QMatrix4x4 groundMVP = mProjectionMatrix * mViewMatrix * groundMatrix;

       // Update uniform buffer for ground
       VkDeviceSize groundOffset = mUniformBufferInfo[mWindow->currentFrame()].offset;
       err = mDeviceFunctions->vkMapMemory(device, mBufferMemory, groundOffset,
                                         UNIFORM_DATA_SIZE, 0, reinterpret_cast<void **>(&GPUmemPointer));
       memcpy(GPUmemPointer, groundMVP.constData(), 16 * sizeof(float));
       mDeviceFunctions->vkUnmapMemory(device, mBufferMemory);

       // Switch to no-texture pipeline for ground if available
       if (mNoTexturePipeline != VK_NULL_HANDLE) {
           mDeviceFunctions->vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mNoTexturePipeline);
           qDebug() << "Using no-texture pipeline for ground";
       } else {
           // Fall back to regular pipeline
           mDeviceFunctions->vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
           qDebug() << "Falling back to regular pipeline for ground (no-texture pipeline not available)";
       }

       // Draw ground using the same descriptor sets (ignoring texture binding)
       mDeviceFunctions->vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                               &mDescriptorSet[mWindow->currentFrame()], 0, nullptr);
       VkDeviceSize groundVertexOffset = 0;
       mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mGroundBuffer, &groundVertexOffset);
       mDeviceFunctions->vkCmdDraw(cb, 6, 1, 0, 0);  // 6 vertices for ground

       qDebug() << "Drew ground plane with color only (no texture) - HeightMap not available";

       // Switch back to textured pipeline for other objects if needed
       if (mNoTexturePipeline != VK_NULL_HANDLE) {
           mDeviceFunctions->vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
       }
   }

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
        if (!mCollectibles[i].isCollected) {
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
            
            // Check if we should use the model or the basic cube
            if (mUseNPCModel) {
                // Use the loaded OBJ model with vertex and index buffers
                VkDeviceSize npcVertexOffset = 0;
                mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mNPCModelBuffer, &npcVertexOffset);
                mDeviceFunctions->vkCmdBindIndexBuffer(cb, mNPCModelIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
                mDeviceFunctions->vkCmdDrawIndexed(cb, mNPCModelIndexCount, 1, 0, 0, 0);
                qDebug() << "Drew NPC 0 with 3D model at position" << mNPCs[0].position;
            } else {
                // Use the basic cube geometry
                VkDeviceSize npcVertexOffset = 0;
                mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mNPCBuffer1, &npcVertexOffset);
                mDeviceFunctions->vkCmdDraw(cb, 36, 1, 0, 0);  // 36 vertices for cube
                qDebug() << "Drew NPC 0 at position" << mNPCs[0].position << "with basic cube";
            }
            
            renderedNPCs++;
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
            
            // Check if we should use the model or the basic cube
            if (mUseNPCModel) {
                // Use the loaded OBJ model with vertex and index buffers
                VkDeviceSize npcVertexOffset = 0;
                mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mNPCModelBuffer, &npcVertexOffset);
                mDeviceFunctions->vkCmdBindIndexBuffer(cb, mNPCModelIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
                mDeviceFunctions->vkCmdDrawIndexed(cb, mNPCModelIndexCount, 1, 0, 0, 0);
                qDebug() << "Drew NPC 1 with 3D model at position" << mNPCs[1].position;
            } else {
                // Use the basic cube geometry
                VkDeviceSize npcVertexOffset = 0;
                mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mNPCBuffer2, &npcVertexOffset);
                mDeviceFunctions->vkCmdDraw(cb, 36, 1, 0, 0);  // 36 vertices for cube
                qDebug() << "Drew NPC 1 at position" << mNPCs[1].position << "with basic cube";
            }
            
            renderedNPCs++;
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
            
            // Check if we should use the model or the basic cube
            if (mUseNPCModel) {
                // Use the loaded OBJ model with vertex and index buffers
                VkDeviceSize npcVertexOffset = 0;
                mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mNPCModelBuffer, &npcVertexOffset);
                mDeviceFunctions->vkCmdBindIndexBuffer(cb, mNPCModelIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
                mDeviceFunctions->vkCmdDrawIndexed(cb, mNPCModelIndexCount, 1, 0, 0, 0);
                qDebug() << "Drew NPC 2 with 3D model at position" << mNPCs[2].position;
            } else {
                // Use the basic cube geometry
                VkDeviceSize npcVertexOffset = 0;
                mDeviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &mNPCBuffer3, &npcVertexOffset);
                mDeviceFunctions->vkCmdDraw(cb, 36, 1, 0, 0);  // 36 vertices for cube
                qDebug() << "Drew NPC 2 at position" << mNPCs[2].position << "with basic cube";
            }
            
            renderedNPCs++;
        }
    }
    
    qDebug() << "Drew" << renderedNPCs << "NPCs with " << (mUseNPCModel ? "custom 3D model" : "basic cubes");

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
    // If heightmap is enabled, adjust house position to be on top of the terrain
float houseTerrainHeight = 0.0f;
if (mUseHeightMap && mHeightMap.getVertices().size() > 0) {
    // Get height at house position
    houseTerrainHeight = mHeightMap.getHeightAt(mHousePosition.x(), mHousePosition.z());
    
    // Update house Y-position to be on top of the terrain
       mHousePosition.setY(houseTerrainHeight + 0.8f); // Increased offset to ensure visibility
    
    qDebug() << "Positioned house on terrain at height:" << houseTerrainHeight;
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
    if (!mIndoorCollectible.isCollected) {
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
    
    // Update lighting based on camera position
    updateLightingData();
    
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
    // Try to open the file using the standard path
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Failed to read shader %s, trying alternate paths...", qPrintable(name));
        
        // Try without the colon prefix (local file)
        QString localPath = name;
        if (localPath.startsWith(":/")) {
            localPath = localPath.mid(2); // Remove the ":/" prefix
        }
        
        QFile localFile(localPath);
        if (!localFile.open(QIODevice::ReadOnly)) {
            // Try assets/shaders directory
            QString assetsPath = "assets/shaders/" + QFileInfo(localPath).fileName();
            QFile assetsFile(assetsPath);
            
            if (!assetsFile.open(QIODevice::ReadOnly)) {
                qWarning("Failed to read shader from all possible paths: %s", qPrintable(name));
                return VK_NULL_HANDLE;
            }
            
            QByteArray blob = assetsFile.readAll();
            assetsFile.close();
            
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
            
            qDebug("Successfully loaded shader from assets path: %s", qPrintable(assetsPath));
            return shaderModule;
        }
        
        QByteArray blob = localFile.readAll();
        localFile.close();
        
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
        
        qDebug("Successfully loaded shader from local path: %s", qPrintable(localPath));
        return shaderModule;
    }
    
    // Original path worked
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
    
    qDebug("Successfully loaded shader from original path: %s", qPrintable(name));
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
    
    if (mLightingBuffer != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(mWindow->device(), mLightingBuffer, nullptr);
        mLightingBuffer = VK_NULL_HANDLE;
    }
    if (mLightingBufferMemory != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(mWindow->device(), mLightingBufferMemory, nullptr);
        mLightingBufferMemory = VK_NULL_HANDLE;
    }
}

void RenderWindow::releaseResources()
{
    qDebug("\n ***************************** releaseResources ******************************************* \n");

    VkDevice device = mWindow->device();

    // -------------------------------------------------------------------------------------------
    // 1. Make sure device is idle before releasing resources
    // -------------------------------------------------------------------------------------------
    mDeviceFunctions->vkDeviceWaitIdle(device);

    // -------------------------------------------------------------------------------------------
    // 2. Destroy descriptor set layouts and pools
    // -------------------------------------------------------------------------------------------
    if (mDescriptorSetLayout != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);
        mDescriptorSetLayout = VK_NULL_HANDLE;
    }

    if (mDescriptorPool != VK_NULL_HANDLE) {
        // Note: Destroying the descriptor pool automatically frees all descriptor sets
        // allocated from it
        mDeviceFunctions->vkDestroyDescriptorPool(device, mDescriptorPool, nullptr);
        mDescriptorPool = VK_NULL_HANDLE;
    }

    // -------------------------------------------------------------------------------------------
    // 3. Destroy pipeline resources
    // -------------------------------------------------------------------------------------------
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

    // -------------------------------------------------------------------------------------------
    // 4. Destroy texture resources
    // -------------------------------------------------------------------------------------------
    if (mTextureSampler != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroySampler(device, mTextureSampler, nullptr);
        mTextureSampler = VK_NULL_HANDLE;
    }
    
    if (mTextureImageView != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyImageView(device, mTextureImageView, nullptr);
        mTextureImageView = VK_NULL_HANDLE;
    }
    
    if (mTextureImage != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyImage(device, mTextureImage, nullptr);
        mTextureImage = VK_NULL_HANDLE;
    }
// No-texture pipeline
if (mNoTexturePipeline != VK_NULL_HANDLE) {
    mDeviceFunctions->vkDestroyPipeline(device, mNoTexturePipeline, nullptr);
    mNoTexturePipeline = VK_NULL_HANDLE;
}
    // -------------------------------------------------------------------------------------------
    // 5. Destroy descriptor set layouts and pools
    // -------------------------------------------------------------------------------------------
    if (mDescriptorSetLayout != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);
        mDescriptorSetLayout = VK_NULL_HANDLE;
    }

    if (mDescriptorPool != VK_NULL_HANDLE) {
        // Note: Destroying the descriptor pool automatically frees all descriptor sets
        // allocated from it
        mDeviceFunctions->vkDestroyDescriptorPool(device, mDescriptorPool, nullptr);
        mDescriptorPool = VK_NULL_HANDLE;
    }

    // -------------------------------------------------------------------------------------------
    // 6. Destroy all buffers
    // -------------------------------------------------------------------------------------------
    // Main uniform buffer
    if (mBuffer != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mBuffer, nullptr);
        mBuffer = VK_NULL_HANDLE;
    }

    // Ground buffers
    if (mGroundBuffer != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mGroundBuffer, nullptr);
        mGroundBuffer = VK_NULL_HANDLE;
    }

    // Player buffers
    if (mPlayerBuffer != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mPlayerBuffer, nullptr);
        mPlayerBuffer = VK_NULL_HANDLE;
    }

    // Collectible buffers
    if (mCollectibleBuffer != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mCollectibleBuffer, nullptr);
        mCollectibleBuffer = VK_NULL_HANDLE;
    }

    // NPC buffers
    if (mNPCBuffer != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mNPCBuffer, nullptr);
        mNPCBuffer = VK_NULL_HANDLE;
    }
    
    // NPC colored buffers
    if (mNPCBuffer1 != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mNPCBuffer1, nullptr);
        mNPCBuffer1 = VK_NULL_HANDLE;
    }
    
    if (mNPCBuffer2 != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mNPCBuffer2, nullptr);
        mNPCBuffer2 = VK_NULL_HANDLE;
    }
    
    if (mNPCBuffer3 != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mNPCBuffer3, nullptr);
        mNPCBuffer3 = VK_NULL_HANDLE;
    }

    // House buffers
    if (mHouseWallsBuffer != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mHouseWallsBuffer, nullptr);
        mHouseWallsBuffer = VK_NULL_HANDLE;
    }

    if (mHouseDoorBuffer != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mHouseDoorBuffer, nullptr);
        mHouseDoorBuffer = VK_NULL_HANDLE;
    }

    if (mHouseRoofBuffer != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mHouseRoofBuffer, nullptr);
        mHouseRoofBuffer = VK_NULL_HANDLE;
    }
    
    // Indoor buffers
    if (mIndoorWallsBuffer != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mIndoorWallsBuffer, nullptr);
        mIndoorWallsBuffer = VK_NULL_HANDLE;
    }
    
    if (mExitDoorBuffer != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mExitDoorBuffer, nullptr);
        mExitDoorBuffer = VK_NULL_HANDLE;
    }

    // Crate cube buffers
    if (mCrateCubeBuffer != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mCrateCubeBuffer, nullptr);
        mCrateCubeBuffer = VK_NULL_HANDLE;
    }
    
    if (mCrateCubeIndexBuffer != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mCrateCubeIndexBuffer, nullptr);
        mCrateCubeIndexBuffer = VK_NULL_HANDLE;
    }

    // Lighting buffer
    if (mLightingBuffer != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyBuffer(device, mLightingBuffer, nullptr);
        mLightingBuffer = VK_NULL_HANDLE;
    }

    // -------------------------------------------------------------------------------------------
    // 7. Free all memory allocations
    // -------------------------------------------------------------------------------------------
    // Main uniform buffer memory
    if (mBufferMemory != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mBufferMemory, nullptr);
        mBufferMemory = VK_NULL_HANDLE;
    }

    // Ground buffer memory
    if (mGroundBufferMemory != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mGroundBufferMemory, nullptr);
        mGroundBufferMemory = VK_NULL_HANDLE;
    }

    // Player buffer memory
    if (mPlayerBufferMemory != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mPlayerBufferMemory, nullptr);
        mPlayerBufferMemory = VK_NULL_HANDLE;
    }

    // Collectible buffer memory
    if (mCollectibleBufferMemory != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mCollectibleBufferMemory, nullptr);
        mCollectibleBufferMemory = VK_NULL_HANDLE;
    }

    // NPC buffer memory
    if (mNPCBufferMemory != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mNPCBufferMemory, nullptr);
        mNPCBufferMemory = VK_NULL_HANDLE;
    }
    
    // NPC colored buffer memory
    if (mNPCBufferMemory1 != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mNPCBufferMemory1, nullptr);
        mNPCBufferMemory1 = VK_NULL_HANDLE;
    }
    
    if (mNPCBufferMemory2 != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mNPCBufferMemory2, nullptr);
        mNPCBufferMemory2 = VK_NULL_HANDLE;
    }
    
    if (mNPCBufferMemory3 != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mNPCBufferMemory3, nullptr);
        mNPCBufferMemory3 = VK_NULL_HANDLE;
    }

    // House buffer memory
    if (mHouseWallsBufferMemory != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mHouseWallsBufferMemory, nullptr);
        mHouseWallsBufferMemory = VK_NULL_HANDLE;
    }

    if (mHouseDoorBufferMemory != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mHouseDoorBufferMemory, nullptr);
        mHouseDoorBufferMemory = VK_NULL_HANDLE;
    }

    if (mHouseRoofBufferMemory != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mHouseRoofBufferMemory, nullptr);
        mHouseRoofBufferMemory = VK_NULL_HANDLE;
    }

    // Indoor buffer memory
    if (mIndoorWallsBufferMemory != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mIndoorWallsBufferMemory, nullptr);
        mIndoorWallsBufferMemory = VK_NULL_HANDLE;
    }
    
    if (mExitDoorBufferMemory != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mExitDoorBufferMemory, nullptr);
        mExitDoorBufferMemory = VK_NULL_HANDLE;
    }

    // Lighting buffer memory
    if (mLightingBufferMemory != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mLightingBufferMemory, nullptr);
        mLightingBufferMemory = VK_NULL_HANDLE;
    }
    
    // Texture image memory
    if (mTextureImageMemory != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mTextureImageMemory, nullptr);
        mTextureImageMemory = VK_NULL_HANDLE;
    }
    
    // -------------------------------------------------------------------------------------------
    // 8. Clean up command pools
    // -------------------------------------------------------------------------------------------
    if (mTempCommandPool != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyCommandPool(device, mTempCommandPool, nullptr);
        mTempCommandPool = VK_NULL_HANDLE;
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
    mIndoorCollectible.isCollected = false;

    qDebug() << "Initialized 6 outdoor and 1 indoor collectibles";

    // If heightmap is enabled, adjust collectible heights to be on top of the terrain
    if (mUseHeightMap && mHeightMap.getVertices().size() > 0) {
        qDebug() << "Adjusting collectible heights based on heightmap...";
        for (int i = 0; i < mCollectibles.size(); i++) {
            // Get height at collectible position
            float terrainHeight = mHeightMap.getHeightAt(mCollectibles[i].position.x(), mCollectibles[i].position.z());
            
            // Place collectible above the terrain with an offset to make it clearly visible
            mCollectibles[i].position.setY(terrainHeight + 4.0f); // Increased offset for better visibility
            
            qDebug() << "Placed collectible" << i << "at height:" << mCollectibles[i].position.y() 
                     << "over terrain height:" << terrainHeight;
        }
    } else {
        qDebug() << "WARNING: Not adjusting collectible heights - heightmap condition not met";
    }
}

void RenderWindow::checkCollectibleCollisions()
{
    const float collectionDistance = 2.0f; // Increased distance for easier collection
    bool collectedAny = false;
    
    for (int i = 0; i < mCollectibles.size(); ++i) {
        if (!mCollectibles[i].isCollected) {
            // For collision, only check X and Z coordinates since Y is different by design
            QVector3D playerXZ(mPlayerPosition.x(), 0.0f, mPlayerPosition.z());
            QVector3D collectibleXZ(mCollectibles[i].position.x(), 0.0f, mCollectibles[i].position.z());
            
            float distance = (playerXZ - collectibleXZ).length();
            
            if (distance < collectionDistance) {
                // Collect the item
                mCollectibles[i].isCollected = true;
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
        // If heightmap is enabled, place NPC on top of terrain
if (mUseHeightMap && mHeightMap.getVertices().size() > 0) {
    // Get height at current position using the enhanced barycentric coordinate method
    float terrainHeight = mHeightMap.getHeightAt(mNPCs[i].position.x(), mNPCs[i].position.z());
    
    // Set NPC height to terrain height plus a small offset
    mNPCs[i].position.setY(terrainHeight + 1.0f); // Increase height for visibility
    } 
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
    qDebug() << "Checking door proximity - Player:" << mPlayerPosition << "Door:" << doorPosition  << "Distance:" << distance << "Door open:" << mDoorOpen;
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
   float entryDistance = 2.0f;
float doorwayWidth = 2.0f;
if (relativePos.length() < entryDistance && 
    std::abs(relativePos.x()) < doorwayWidth &&
    relativePos.z() < 1.0f && relativePos.z() > -1.0f) {
        
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
    if (mIndoorCollectible.isCollected) {
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
        mIndoorCollectible.isCollected = true;
        
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

// Create a default white texture when no texture is loaded
void RenderWindow::createDefaultTexture(VkDevice device)
{
    // Create a simple 2x2 white texture
    const uint32_t texWidth = 2;
    const uint32_t texHeight = 2;
    const VkDeviceSize imageSize = texWidth * texHeight * 4; // RGBA
    
    // Create white texture data (RGBA)
    unsigned char* texData = new unsigned char[imageSize];
    for (uint32_t i = 0; i < imageSize; i += 4) {
        texData[i] = 255;     // R
        texData[i + 1] = 255; // G
        texData[i + 2] = 255; // B
        texData[i + 3] = 255; // A
    }
    
    // Create staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = imageSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VkResult err = mDeviceFunctions->vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer);
    if (err != VK_SUCCESS)
        qFatal("Failed to create staging buffer: %d", err);
    
    VkMemoryRequirements memRequirements;
    mDeviceFunctions->vkGetBufferMemoryRequirements(device, stagingBuffer, &memRequirements);
    
    // Allocate memory for staging buffer with HOST_VISIBLE for CPU access
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    
    VkPhysicalDeviceMemoryProperties textureMemProperties;
    mWindow->vulkanInstance()->functions()->vkGetPhysicalDeviceMemoryProperties(mWindow->physicalDevice(), &textureMemProperties);
    
    // Use HOST_VISIBLE memory for staging buffer so we can map it
    allocInfo.memoryTypeIndex = getMemoryTypeIndex(
        textureMemProperties,
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    
    err = mDeviceFunctions->vkAllocateMemory(device, &allocInfo, nullptr, &stagingBufferMemory);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate staging buffer memory: %d", err);
    
    err = mDeviceFunctions->vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind staging buffer memory: %d", err);
    
    // Copy data to staging buffer using mapped memory
    void* data;
    err = mDeviceFunctions->vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    if (err != VK_SUCCESS)
        qFatal("Failed to map memory: %d", err);
    
    memcpy(data, texData, static_cast<size_t>(imageSize));
    mDeviceFunctions->vkUnmapMemory(device, stagingBufferMemory);
    
    delete[] texData;
    
    // Create the texture image
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = texWidth;
    imageInfo.extent.height = texHeight;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    
    err = mDeviceFunctions->vkCreateImage(device, &imageInfo, nullptr, &mTextureImage);
    if (err != VK_SUCCESS)
        qFatal("Failed to create texture image: %d", err);
    
    // Allocate memory for texture image
    VkMemoryRequirements memReq;
    mDeviceFunctions->vkGetImageMemoryRequirements(device, mTextureImage, &memReq);
    
    VkMemoryAllocateInfo memAllocInfo = {};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = memReq.size;
    
    VkPhysicalDeviceMemoryProperties imageMemProperties;
    mWindow->vulkanInstance()->functions()->vkGetPhysicalDeviceMemoryProperties(mWindow->physicalDevice(), &imageMemProperties);
    
    // Always use device-local memory for the final texture image since we'll copy to it
    memAllocInfo.memoryTypeIndex = getMemoryTypeIndex(
        imageMemProperties,
        memReq.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT // Device-local is better for GPU performance
    );
    
    err = mDeviceFunctions->vkAllocateMemory(device, &memAllocInfo, nullptr, &mTextureImageMemory);
    if (err != VK_SUCCESS)
        qFatal("Failed to allocate texture image memory: %d", err);
    
    err = mDeviceFunctions->vkBindImageMemory(device, mTextureImage, mTextureImageMemory, 0);
    if (err != VK_SUCCESS)
        qFatal("Failed to bind texture image memory: %d", err);
    
    // Use our custom command buffer for one-time operations
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = mTextureImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    
    mDeviceFunctions->vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
    
    // Copy buffer to image
    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { texWidth, texHeight, 1 };
    
    mDeviceFunctions->vkCmdCopyBufferToImage(
        commandBuffer,
        stagingBuffer,
        mTextureImage,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
    
    // Transition image layout to shader read only optimal
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    
    mDeviceFunctions->vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
    
    // Submit and clean up the command buffer
    endSingleTimeCommands(commandBuffer);
    
    // Create texture image view
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = mTextureImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    
    err = mDeviceFunctions->vkCreateImageView(device, &viewInfo, nullptr, &mTextureImageView);
    if (err != VK_SUCCESS)
        qFatal("Failed to create texture image view: %d", err);
    
    // Create texture sampler
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    
    err = mDeviceFunctions->vkCreateSampler(device, &samplerInfo, nullptr, &mTextureSampler);
    if (err != VK_SUCCESS)
        qFatal("Failed to create texture sampler: %d", err);
    
    // Clean up staging resources
    mDeviceFunctions->vkDestroyBuffer(device, stagingBuffer, nullptr);
    mDeviceFunctions->vkFreeMemory(device, stagingBufferMemory, nullptr);
}

VkCommandBuffer RenderWindow::beginSingleTimeCommands() {
    VkDevice device = mWindow->device();
    
    // Create command pool if it doesn't exist
    if (mTempCommandPool == VK_NULL_HANDLE) {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        poolInfo.queueFamilyIndex = mWindow->graphicsQueueFamilyIndex();
        
        VkResult err = mDeviceFunctions->vkCreateCommandPool(device, &poolInfo, nullptr, &mTempCommandPool);
        if (err != VK_SUCCESS) {
            qFatal("Failed to create temporary command pool: %d", err);
        }
    }
    
    // Allocate command buffer
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = mTempCommandPool;
    allocInfo.commandBufferCount = 1;
    
    VkCommandBuffer commandBuffer;
    VkResult err = mDeviceFunctions->vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
    if (err != VK_SUCCESS) {
        qFatal("Failed to allocate command buffer: %d", err);
    }
    
    // Begin command buffer
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    err = mDeviceFunctions->vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (err != VK_SUCCESS) {
        qFatal("Failed to begin command buffer: %d", err);
    }
    
    return commandBuffer;
}

void RenderWindow::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    VkDevice device = mWindow->device();
    
    // End command buffer
    VkResult err = mDeviceFunctions->vkEndCommandBuffer(commandBuffer);
    if (err != VK_SUCCESS) {
        qFatal("Failed to end command buffer: %d", err);
    }
    
    // Submit command buffer
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    err = mDeviceFunctions->vkQueueSubmit(mWindow->graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    if (err != VK_SUCCESS) {
        qFatal("Failed to submit queue: %d", err);
    }
    
    // Wait for the GPU to finish execution
    err = mDeviceFunctions->vkQueueWaitIdle(mWindow->graphicsQueue());
    if (err != VK_SUCCESS) {
        qFatal("Failed to wait for queue idle: %d", err);
    }
    
    // Free command buffer
    mDeviceFunctions->vkFreeCommandBuffers(device, mTempCommandPool, 1, &commandBuffer);
}

// Load a texture image from file
void RenderWindow::createTextureImage()
{
    // Resource debugging test
    qDebug() << "\n========= RESOURCE TEST =========";
    if (QFile::exists(":/assets/textures/wood_texture.jpg")) {
        qDebug() << "PASS: Texture resource exists!";
    } else {
        qDebug() << "FAIL: Texture resource NOT found in Qt resource system";
        qDebug() << "Available resources at root:";
        QDir resourceDir(":/");
        QStringList entries = resourceDir.entryList(QDir::AllEntries);
        for (const QString& entry : entries) {
            qDebug() << " - " << entry;
        }
        
        // Try to check if assets directory exists
        if (QDir(":/assets").exists()) {
            qDebug() << "Assets directory exists, contents:";
            QStringList assetEntries = QDir(":/assets").entryList(QDir::AllEntries);
            for (const QString& entry : assetEntries) {
                qDebug() << " - " << entry;
            }
            
            // If assets exists, check textures subdirectory
            if (QDir(":/assets/textures").exists()) {
                qDebug() << "Textures directory exists, contents:";
                QStringList textureEntries = QDir(":/assets/textures").entryList(QDir::AllEntries);
                for (const QString& entry : textureEntries) {
                    qDebug() << " - " << entry;
                }
            } else {
                qDebug() << "Textures directory does not exist in resource system";
            }
        } else {
            qDebug() << "Assets directory does not exist in resource system";
        }
    }
    qDebug() << "================================\n";

    VkDevice device = mWindow->device();
    
    // Define the file path for wood texture
    QString texturePath = ":/assets/textures/wood_texture.jpg";
    
    // Check if the file exists first
    QFileInfo fileInfo(texturePath);
    if (!fileInfo.exists()) {
        qWarning() << "Texture file does not exist at path:" << texturePath;
        qWarning() << "Absolute path attempted:" << fileInfo.absoluteFilePath();
        qWarning() << "Working directory:" << QDir::currentPath();
        
        // Try alternative paths
        QStringList alternativePaths = {
            "./assets/textures/wood_texture.jpg",
            "../assets/textures/wood_texture.jpg",
            ":/assets/textures/wood_texture.jpg",
            ":/textures/wood_texture.jpg",
            ":/wood_texture.jpg"
        };
        
        bool found = false;
        for (const QString& altPath : alternativePaths) {
            QFileInfo altInfo(altPath);
            if (altInfo.exists()) {
                texturePath = altPath;
                qDebug() << "Found texture at alternative path:" << texturePath;
                found = true;
                break;
            }
        }
        
        if (!found) {
            qWarning() << "No texture found in alternative paths. Creating default texture.";
            // Fall back to default texture
            createDefaultTexture(device);
            return;
        }
    }
    
    QImage image(texturePath);
    
    if (image.isNull()) {
        qWarning() << "Failed to load texture:" << texturePath;
        // Fall back to default texture
        createDefaultTexture(device);
        return;
    }
    
    // Convert to RGBA format
    image = image.convertToFormat(QImage::Format_RGBA8888);
    
    const uint32_t texWidth = image.width();
    const uint32_t texHeight = image.height();
    const VkDeviceSize imageSize = texWidth * texHeight * 4; // RGBA
    
    // Create staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = imageSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VkResult err = mDeviceFunctions->vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer);
    if (err != VK_SUCCESS) {
        qFatal("Failed to create staging buffer: %d", err);
    }
    
    VkMemoryRequirements memRequirements;
    mDeviceFunctions->vkGetBufferMemoryRequirements(device, stagingBuffer, &memRequirements);
    
    // Allocate memory for staging buffer with HOST_VISIBLE for CPU access
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    
    VkPhysicalDeviceMemoryProperties textureMemProperties;
    mWindow->vulkanInstance()->functions()->vkGetPhysicalDeviceMemoryProperties(mWindow->physicalDevice(), &textureMemProperties);
    
    // Use HOST_VISIBLE memory for staging buffer so we can map it
    allocInfo.memoryTypeIndex = getMemoryTypeIndex(
        textureMemProperties,
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    
    err = mDeviceFunctions->vkAllocateMemory(device, &allocInfo, nullptr, &stagingBufferMemory);
    if (err != VK_SUCCESS) {
        qFatal("Failed to allocate staging buffer memory: %d", err);
    }
    
    err = mDeviceFunctions->vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);
    if (err != VK_SUCCESS) {
        qFatal("Failed to bind staging buffer memory: %d", err);
    }
    
    // Copy data to staging buffer using mapped memory
    void* data;
    err = mDeviceFunctions->vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    if (err != VK_SUCCESS) {
        qFatal("Failed to map memory: %d", err);
    }
    
    // Copy the image data to the staging buffer
    memcpy(data, image.constBits(), static_cast<size_t>(imageSize));
    mDeviceFunctions->vkUnmapMemory(device, stagingBufferMemory);
    
    // Clean up existing texture if it exists
    if (mTextureImage != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyImage(device, mTextureImage, nullptr);
        mTextureImage = VK_NULL_HANDLE;
    }
    
    if (mTextureImageMemory != VK_NULL_HANDLE) {
        mDeviceFunctions->vkFreeMemory(device, mTextureImageMemory, nullptr);
        mTextureImageMemory = VK_NULL_HANDLE;
    }
    
    // Create texture image with optimal tiling for GPU access
    createImage(
        texWidth, 
        texHeight, 
        VK_FORMAT_R8G8B8A8_UNORM, 
        VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        mTextureImage, 
        mTextureImageMemory
    );
    
    // Transition image layout for copy operation
    transitionImageLayout(
        mTextureImage, 
        VK_FORMAT_R8G8B8A8_UNORM, 
        VK_IMAGE_LAYOUT_UNDEFINED, 
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );
    
    // Copy data from staging buffer to image
    copyBufferToImage(stagingBuffer, mTextureImage, texWidth, texHeight);
    
    // Transition image layout for shader access
    transitionImageLayout(
        mTextureImage, 
        VK_FORMAT_R8G8B8A8_UNORM, 
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );
    
    // Cleanup staging resources
    mDeviceFunctions->vkDestroyBuffer(device, stagingBuffer, nullptr);
    mDeviceFunctions->vkFreeMemory(device, stagingBufferMemory, nullptr);
    
    qDebug() << "Texture loaded successfully:" << texturePath;
}

// Create texture image view
void RenderWindow::createTextureImageView() 
{
    VkDevice device = mWindow->device();
    
    // Clean up old view if it exists
    if (mTextureImageView != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroyImageView(device, mTextureImageView, nullptr);
        mTextureImageView = VK_NULL_HANDLE;
    }
    
    mTextureImageView = createImageView(mTextureImage, VK_FORMAT_R8G8B8A8_UNORM);
}

// Create image view helper function
VkImageView RenderWindow::createImageView(VkImage image, VkFormat format) 
{
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    
    VkImageView imageView;
    VkResult err = mDeviceFunctions->vkCreateImageView(mWindow->device(), &viewInfo, nullptr, &imageView);
    if (err != VK_SUCCESS) {
        qFatal("Failed to create texture image view: %d", err);
    }
    
    return imageView;
}

// Create texture sampler
void RenderWindow::createTextureSampler() 
{
    VkDevice device = mWindow->device();
    
    // Clean up old sampler if it exists
    if (mTextureSampler != VK_NULL_HANDLE) {
        mDeviceFunctions->vkDestroySampler(device, mTextureSampler, nullptr);
        mTextureSampler = VK_NULL_HANDLE;
    }
    
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    
    VkResult err = mDeviceFunctions->vkCreateSampler(device, &samplerInfo, nullptr, &mTextureSampler);
    if (err != VK_SUCCESS)
        qFatal("Failed to create texture sampler: %d", err);
}

// Handle image layout transitions for proper texture usage
void RenderWindow::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) 
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    
    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
    
    // Define access masks and pipeline stages based on old/new layouts
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } 
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        qFatal("Unsupported layout transition!");
    }
    
    mDeviceFunctions->vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
    
    endSingleTimeCommands(commandBuffer);
}

// Copy buffer data to an image (for texture loading)
void RenderWindow::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) 
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    
    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };
    
    mDeviceFunctions->vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
    
    endSingleTimeCommands(commandBuffer);
}

// Create a VkImage with the specified parameters
void RenderWindow::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                            VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                            VkImage& image, VkDeviceMemory& imageMemory) 
{
    VkDevice device = mWindow->device();
    
    // Create image
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VkResult err = mDeviceFunctions->vkCreateImage(device, &imageInfo, nullptr, &image);
    if (err != VK_SUCCESS) {
        qFatal("Failed to create image: %d", err);
    }
    
    // Get memory requirements
    VkMemoryRequirements memRequirements;
    mDeviceFunctions->vkGetImageMemoryRequirements(device, image, &memRequirements);
    
    // Allocate memory
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    
    // Get physical device memory properties
    VkPhysicalDeviceMemoryProperties memProperties;
    mWindow->vulkanInstance()->functions()->vkGetPhysicalDeviceMemoryProperties(mWindow->physicalDevice(), &memProperties);
    
    allocInfo.memoryTypeIndex = getMemoryTypeIndex(memProperties, memRequirements.memoryTypeBits, properties);
    
    err = mDeviceFunctions->vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory);
    if (err != VK_SUCCESS) {
        qFatal("Failed to allocate image memory: %d", err);
    }
    
    // Bind memory to image
    err = mDeviceFunctions->vkBindImageMemory(device, image, imageMemory, 0);
    if (err != VK_SUCCESS) {
        qFatal("Failed to bind image memory: %d", err);
    }
}

void RenderWindow::updateLightingData()
{
    // Update view position to match camera position for accurate lighting calculations
    // We use the player position for the view position
    mLightingData.viewPosition[0] = mPlayerPosition.x();
    mLightingData.viewPosition[1] = mPlayerPosition.y();
    mLightingData.viewPosition[2] = mPlayerPosition.z();
    
    // Make a light that follows the player (slightly above and behind)
    mLightingData.lightPosition[0] = mPlayerPosition.x() + 1.0f;
    mLightingData.lightPosition[1] = mPlayerPosition.y() + 2.0f;
    mLightingData.lightPosition[2] = mPlayerPosition.z() + 1.0f;
    
    // Set good lighting parameters
    mLightingData.lightColor[0] = 1.0f;  // Full white light
    mLightingData.lightColor[1] = 1.0f;
    mLightingData.lightColor[2] = 1.0f;
    mLightingData.ambientStrength = 0.3f;  // Increase ambient so objects aren't too dark
    mLightingData.specularStrength = 0.5f;
    mLightingData.shininess = 32.0f;
    
    // Update the lighting buffer with new data
    VkDevice device = mWindow->device();
    
    // Create staging buffer for the updated data
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    // Create staging buffer for lighting data
    VkBufferCreateInfo stagingBufferInfo = {};
    stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferInfo.size = sizeof(LightingInfo);
    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VkResult err = mDeviceFunctions->vkCreateBuffer(device, &stagingBufferInfo, nullptr, &stagingBuffer);
    if (err != VK_SUCCESS) {
        qWarning("Failed to create lighting staging buffer: %d", err);
        return;
    }
    
    VkMemoryRequirements stagingMemReq;
    mDeviceFunctions->vkGetBufferMemoryRequirements(device, stagingBuffer, &stagingMemReq);
    
    VkMemoryAllocateInfo stagingAllocInfo = {};
    stagingAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    stagingAllocInfo.allocationSize = stagingMemReq.size;
    
    // Get memory properties
    VkPhysicalDeviceMemoryProperties memProperties;
    mWindow->vulkanInstance()->functions()->vkGetPhysicalDeviceMemoryProperties(mWindow->physicalDevice(), &memProperties);
    
    stagingAllocInfo.memoryTypeIndex = getMemoryTypeIndex(memProperties, 
                                                         stagingMemReq.memoryTypeBits, 
                                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    err = mDeviceFunctions->vkAllocateMemory(device, &stagingAllocInfo, nullptr, &stagingBufferMemory);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, stagingBuffer, nullptr);
        qWarning("Failed to allocate lighting staging buffer memory: %d", err);
        return;
    }
    
    err = mDeviceFunctions->vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, stagingBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingBufferMemory, nullptr);
        qWarning("Failed to bind lighting staging buffer memory: %d", err);
        return;
    }
    
    // Copy updated lighting data to staging buffer
    void* data;
    err = mDeviceFunctions->vkMapMemory(device, stagingBufferMemory, 0, sizeof(LightingInfo), 0, &data);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, stagingBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingBufferMemory, nullptr);
        qWarning("Failed to map lighting staging buffer memory: %d", err);
        return;
    }
    
    memcpy(data, &mLightingData, sizeof(LightingInfo));
    mDeviceFunctions->vkUnmapMemory(device, stagingBufferMemory);
    
    // Copy from staging buffer to device local buffer
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    
    VkBufferCopy copyRegion = {};
    copyRegion.size = sizeof(LightingInfo);
    mDeviceFunctions->vkCmdCopyBuffer(commandBuffer, stagingBuffer, mLightingBuffer, 1, &copyRegion);
    
    endSingleTimeCommands(commandBuffer);
    
    // Clean up staging buffer
    mDeviceFunctions->vkDestroyBuffer(device, stagingBuffer, nullptr);
    mDeviceFunctions->vkFreeMemory(device, stagingBufferMemory, nullptr);
    
    // Debug output
    qDebug() << "Updated lighting data - View position:" 
             << mLightingData.viewPosition[0] << mLightingData.viewPosition[1] << mLightingData.viewPosition[2]
             << "Light position:" 
             << mLightingData.lightPosition[0] << mLightingData.lightPosition[1] << mLightingData.lightPosition[2];
}

void RenderWindow::loadNPCModel() {
    VkDevice device = mWindow->device();
    
    qDebug() << "Loading 3D model for NPCs: CrateCube.obj";
    
    std::vector<ObjLoader::Vertex> vertices;
    std::vector<uint32_t> indices;
    
    // Load the model from the models directory
    QString modelPath = ":/assets/models/CrateCube.obj";
    
    if (!ObjLoader::loadObjFile(modelPath, vertices, indices)) {
        qWarning() << "Failed to load NPC model from" << modelPath << ", using fallback geometry";
        return; // Use existing NPC geometry as fallback
    }
    
    // Save index count for draw call
    mNPCModelIndexCount = static_cast<uint32_t>(indices.size());
    
    VkDeviceSize vertexBufferSize = sizeof(ObjLoader::Vertex) * vertices.size();
    VkDeviceSize indexBufferSize = sizeof(uint32_t) * indices.size();
    
    // Create staging buffer for vertices
    VkBuffer stagingVertexBuffer;
    VkDeviceMemory stagingVertexBufferMemory;
    createBuffer(
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingVertexBuffer,
        stagingVertexBufferMemory
    );
    
    // Map and copy vertex data
    void* vertexData;
    mDeviceFunctions->vkMapMemory(device, stagingVertexBufferMemory, 0, vertexBufferSize, 0, &vertexData);
    memcpy(vertexData, vertices.data(), static_cast<size_t>(vertexBufferSize));
    mDeviceFunctions->vkUnmapMemory(device, stagingVertexBufferMemory);
    
    // Create staging buffer for indices
    VkBuffer stagingIndexBuffer;
    VkDeviceMemory stagingIndexBufferMemory;
    createBuffer(
        indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingIndexBuffer,
        stagingIndexBufferMemory
    );
    
    // Map and copy index data
    void* indexData;
    mDeviceFunctions->vkMapMemory(device, stagingIndexBufferMemory, 0, indexBufferSize, 0, &indexData);
    memcpy(indexData, indices.data(), static_cast<size_t>(indexBufferSize));
    mDeviceFunctions->vkUnmapMemory(device, stagingIndexBufferMemory);
    
    // Create device local vertex buffer
    createBuffer(
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        mNPCModelBuffer,
        mNPCModelBufferMemory
    );
    
    // Create device local index buffer
    createBuffer(
        indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        mNPCModelIndexBuffer,
        mNPCModelIndexBufferMemory
    );
    
    // Copy buffers
    copyBuffer(stagingVertexBuffer, mNPCModelBuffer, vertexBufferSize);
    copyBuffer(stagingIndexBuffer, mNPCModelIndexBuffer, indexBufferSize);
    
    // Clean up staging resources
    mDeviceFunctions->vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
    mDeviceFunctions->vkFreeMemory(device, stagingVertexBufferMemory, nullptr);
    mDeviceFunctions->vkDestroyBuffer(device, stagingIndexBuffer, nullptr);
    mDeviceFunctions->vkFreeMemory(device, stagingIndexBufferMemory, nullptr);
    
    // Set the flag to use the model
    mUseNPCModel = true;
    
    qDebug() << "Successfully loaded NPC model with" << vertices.size() << "vertices and" 
             << mNPCModelIndexCount << "indices";
}

void RenderWindow::loadCrateCubeModel() {
    VkDevice device = mWindow->device();
    
    qDebug() << "Loading 3D model for Crate: assets/models/CrateCube.obj";
    
    // Load the model using new ObjLoader class
    QString modelPath = "assets/models/CrateCube.obj";
    ObjModel model = ObjLoader::loadObj(modelPath);
    
    // Get memory properties once and reuse
    VkPhysicalDeviceMemoryProperties memProperties;
    mWindow->vulkanInstance()->functions()->vkGetPhysicalDeviceMemoryProperties(mWindow->physicalDevice(), &memProperties);
    
    if (model.vertices.isEmpty()) {
        qWarning() << "Failed to load Crate model from" << modelPath << ", using fallback geometry";
        return; // Use existing geometry as fallback
    }
    
    // Save index count for draw call
    mCrateCubeIndexCount = static_cast<uint32_t>(model.indices.size());
    
    // Convert ObjVertex to raw vertex data for Vulkan
    QVector<float> vertexData;
    for (const auto& vertex : model.vertices) {
        // Position
        vertexData.append(vertex.position.x());
        vertexData.append(vertex.position.y());
        vertexData.append(vertex.position.z());
        
        // Normal (converted to color)
        QVector3D normal = vertex.normal.normalized();
        vertexData.append(normal.x() * 0.5f + 0.5f); // Convert from [-1,1] to [0,1]
        vertexData.append(normal.y() * 0.5f + 0.5f);
        vertexData.append(normal.z() * 0.5f + 0.5f);
        
        // Texture coordinates
        vertexData.append(vertex.texCoord.x());
        vertexData.append(vertex.texCoord.y());
    }
    
    VkDeviceSize vertexBufferSize = sizeof(float) * vertexData.size();
    VkDeviceSize indexBufferSize = sizeof(uint32_t) * model.indices.size();
    
    // Create staging buffer for vertices
    VkBuffer stagingVertexBuffer;
    VkDeviceMemory stagingVertexBufferMemory;
    
    VkBufferCreateInfo stagingBufferInfo = {};
    stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferInfo.size = vertexBufferSize;
    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VkResult err = mDeviceFunctions->vkCreateBuffer(device, &stagingBufferInfo, nullptr, &stagingVertexBuffer);
    if (err != VK_SUCCESS) {
        qWarning() << "Failed to create staging vertex buffer for Crate model:" << err;
        return;
    }
    
    VkMemoryRequirements memRequirements;
    mDeviceFunctions->vkGetBufferMemoryRequirements(device, stagingVertexBuffer, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = getMemoryTypeIndex(
        memProperties,
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    
    err = mDeviceFunctions->vkAllocateMemory(device, &allocInfo, nullptr, &stagingVertexBufferMemory);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
        qWarning() << "Failed to allocate staging vertex buffer memory for Crate model:" << err;
        return;
    }
    
    err = mDeviceFunctions->vkBindBufferMemory(device, stagingVertexBuffer, stagingVertexBufferMemory, 0);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingVertexBufferMemory, nullptr);
        qWarning() << "Failed to bind staging vertex buffer memory for Crate model:" << err;
        return;
    }
    
    // Map and copy vertex data
    void* vertexData_ptr;
    mDeviceFunctions->vkMapMemory(device, stagingVertexBufferMemory, 0, vertexBufferSize, 0, &vertexData_ptr);
    memcpy(vertexData_ptr, vertexData.constData(), static_cast<size_t>(vertexBufferSize));
    mDeviceFunctions->vkUnmapMemory(device, stagingVertexBufferMemory);
    
    // Create staging buffer for indices
    VkBuffer stagingIndexBuffer;
    VkDeviceMemory stagingIndexBufferMemory;
    
    stagingBufferInfo.size = indexBufferSize;
    
    err = mDeviceFunctions->vkCreateBuffer(device, &stagingBufferInfo, nullptr, &stagingIndexBuffer);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingVertexBufferMemory, nullptr);
        qWarning() << "Failed to create staging index buffer for Crate model:" << err;
        return;
    }
    
    mDeviceFunctions->vkGetBufferMemoryRequirements(device, stagingIndexBuffer, &memRequirements);
    
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = getMemoryTypeIndex(
        memProperties,
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    
    err = mDeviceFunctions->vkAllocateMemory(device, &allocInfo, nullptr, &stagingIndexBufferMemory);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingVertexBufferMemory, nullptr);
        mDeviceFunctions->vkDestroyBuffer(device, stagingIndexBuffer, nullptr);
        qWarning() << "Failed to allocate staging index buffer memory for Crate model:" << err;
        return;
    }
    
    err = mDeviceFunctions->vkBindBufferMemory(device, stagingIndexBuffer, stagingIndexBufferMemory, 0);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingVertexBufferMemory, nullptr);
        mDeviceFunctions->vkDestroyBuffer(device, stagingIndexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingIndexBufferMemory, nullptr);
        qWarning() << "Failed to bind staging index buffer memory for Crate model:" << err;
        return;
    }
    
    // Map and copy index data
    void* indexData;
    mDeviceFunctions->vkMapMemory(device, stagingIndexBufferMemory, 0, indexBufferSize, 0, &indexData);
    memcpy(indexData, model.indices.constData(), static_cast<size_t>(indexBufferSize));
    mDeviceFunctions->vkUnmapMemory(device, stagingIndexBufferMemory);
    
    // Create device local vertex buffer
    VkBufferCreateInfo deviceBufferInfo = {};
    deviceBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    deviceBufferInfo.size = vertexBufferSize;
    deviceBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    deviceBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    err = mDeviceFunctions->vkCreateBuffer(device, &deviceBufferInfo, nullptr, &mCrateCubeBuffer);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingVertexBufferMemory, nullptr);
        mDeviceFunctions->vkDestroyBuffer(device, stagingIndexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingIndexBufferMemory, nullptr);
        qWarning() << "Failed to create device vertex buffer for Crate model:" << err;
        return;
    }
    
    mDeviceFunctions->vkGetBufferMemoryRequirements(device, mCrateCubeBuffer, &memRequirements);
    
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = getMemoryTypeIndex(
        memProperties,
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    
    err = mDeviceFunctions->vkAllocateMemory(device, &allocInfo, nullptr, &mCrateCubeBufferMemory);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingVertexBufferMemory, nullptr);
        mDeviceFunctions->vkDestroyBuffer(device, stagingIndexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingIndexBufferMemory, nullptr);
        mDeviceFunctions->vkDestroyBuffer(device, mCrateCubeBuffer, nullptr);
        qWarning() << "Failed to allocate device vertex buffer memory for Crate model:" << err;
        return;
    }
    
    err = mDeviceFunctions->vkBindBufferMemory(device, mCrateCubeBuffer, mCrateCubeBufferMemory, 0);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingVertexBufferMemory, nullptr);
        mDeviceFunctions->vkDestroyBuffer(device, stagingIndexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingIndexBufferMemory, nullptr);
        mDeviceFunctions->vkDestroyBuffer(device, mCrateCubeBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, mCrateCubeBufferMemory, nullptr);
        qWarning() << "Failed to bind device vertex buffer memory for Crate model:" << err;
        return;
    }
    
    // Create device local index buffer
    deviceBufferInfo.size = indexBufferSize;
    deviceBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    
    err = mDeviceFunctions->vkCreateBuffer(device, &deviceBufferInfo, nullptr, &mCrateCubeIndexBuffer);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingVertexBufferMemory, nullptr);
        mDeviceFunctions->vkDestroyBuffer(device, stagingIndexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingIndexBufferMemory, nullptr);
        mDeviceFunctions->vkDestroyBuffer(device, mCrateCubeBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, mCrateCubeBufferMemory, nullptr);
        qWarning() << "Failed to create device index buffer for Crate model:" << err;
        return;
    }
    
    mDeviceFunctions->vkGetBufferMemoryRequirements(device, mCrateCubeIndexBuffer, &memRequirements);
    
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = getMemoryTypeIndex(
        memProperties,
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
    
    err = mDeviceFunctions->vkAllocateMemory(device, &allocInfo, nullptr, &mCrateCubeIndexBufferMemory);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingVertexBufferMemory, nullptr);
        mDeviceFunctions->vkDestroyBuffer(device, stagingIndexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingIndexBufferMemory, nullptr);
        mDeviceFunctions->vkDestroyBuffer(device, mCrateCubeBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, mCrateCubeBufferMemory, nullptr);
        mDeviceFunctions->vkDestroyBuffer(device, mCrateCubeIndexBuffer, nullptr);
        qWarning() << "Failed to allocate device index buffer memory for Crate model:" << err;
        return;
    }
    
    err = mDeviceFunctions->vkBindBufferMemory(device, mCrateCubeIndexBuffer, mCrateCubeIndexBufferMemory, 0);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingVertexBufferMemory, nullptr);
        mDeviceFunctions->vkDestroyBuffer(device, stagingIndexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, stagingIndexBufferMemory, nullptr);
        mDeviceFunctions->vkDestroyBuffer(device, mCrateCubeBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, mCrateCubeBufferMemory, nullptr);
        mDeviceFunctions->vkDestroyBuffer(device, mCrateCubeIndexBuffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, mCrateCubeIndexBufferMemory, nullptr);
        qWarning() << "Failed to bind device index buffer memory for Crate model:" << err;
        return;
    }
    
    // Copy buffers using command buffer
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    
    VkBufferCopy copyRegion = {};
    copyRegion.size = vertexBufferSize;
    mDeviceFunctions->vkCmdCopyBuffer(commandBuffer, stagingVertexBuffer, mCrateCubeBuffer, 1, &copyRegion);
    
    copyRegion.size = indexBufferSize;
    mDeviceFunctions->vkCmdCopyBuffer(commandBuffer, stagingIndexBuffer, mCrateCubeIndexBuffer, 1, &copyRegion);
    
    endSingleTimeCommands(commandBuffer);
    
    // Clean up staging resources
    mDeviceFunctions->vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
    mDeviceFunctions->vkFreeMemory(device, stagingVertexBufferMemory, nullptr);
    mDeviceFunctions->vkDestroyBuffer(device, stagingIndexBuffer, nullptr);
    mDeviceFunctions->vkFreeMemory(device, stagingIndexBufferMemory, nullptr);
    
    qDebug() << "Successfully loaded Crate model with" << model.vertices.size() << "vertices and" 
             << mCrateCubeIndexCount << "indices";
             }
void RenderWindow::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, 
                               VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkDevice device = mWindow->device();
    
    // Create buffer
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VkResult err = mDeviceFunctions->vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
    if (err != VK_SUCCESS) {
        qWarning() << "Failed to create buffer:" << err;
        return;
    }
    
    // Get memory requirements
    VkMemoryRequirements memRequirements;
    mDeviceFunctions->vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
    
    // Allocate memory
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    
    // Get memory properties for finding memory type index
    VkPhysicalDeviceMemoryProperties memProperties;
    mWindow->vulkanInstance()->functions()->vkGetPhysicalDeviceMemoryProperties(mWindow->physicalDevice(), &memProperties);
    
    allocInfo.memoryTypeIndex = getMemoryTypeIndex(
        memProperties,
        memRequirements.memoryTypeBits,
        properties
    );
    
    err = mDeviceFunctions->vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, buffer, nullptr);
        qWarning() << "Failed to allocate buffer memory:" << err;
        return;
    }
    
    // Bind memory to buffer
    err = mDeviceFunctions->vkBindBufferMemory(device, buffer, bufferMemory, 0);
    if (err != VK_SUCCESS) {
        mDeviceFunctions->vkDestroyBuffer(device, buffer, nullptr);
        mDeviceFunctions->vkFreeMemory(device, bufferMemory, nullptr);
        qWarning() << "Failed to bind buffer memory:" << err;
        return;
    }
}

void RenderWindow::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    // Begin single-time command buffer
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    
    // Record copy command
    VkBufferCopy copyRegion = {};
    copyRegion.size = size;
    mDeviceFunctions->vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    
    // End command buffer and submit it
    endSingleTimeCommands(commandBuffer);
}







