#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <string>
#include <vector>
#include <QVector3D>
#include "ObjLoader.h"

class HeightMap
{
public:
    HeightMap();
    
    // Load a heightmap from an image file
    void makeTerrain(std::string heightMapImage);
    
    // Generate terrain from raw pixel data
    void makeTerrain(unsigned char* textureData, int width, int height);
    
    // Set vertices and indices directly - new methods for procedural generation
    void setVertices(const std::vector<ObjLoader::Vertex>& vertices) { mVertices = vertices; }
    void setIndices(const std::vector<uint32_t>& indices) { mIndices = indices; }
    
    // Helper function to find height at a specific world position (for player movement)
    float getHeightAt(float x, float z);
    
    // Get the generated vertices and indices
    const std::vector<ObjLoader::Vertex>& getVertices() const { return mVertices; }
    const std::vector<uint32_t>& getIndices() const { return mIndices; }
    
private:
    std::vector<ObjLoader::Vertex> mVertices;
    std::vector<uint32_t> mIndices;
    
    int mWidth{ 0 };
    int mHeight{ 0 };
    int mChannels{ 0 };
    
    // Parameters for terrain generation
    float mHorizontalSpacing{ 0.2f };  // Distance between vertices in x and z
    float mHeightSpacing{ 0.02f };     // Scaling factor for height values
    float mHeightPlacement{ 0.0f };    // Base height offset
};

#endif // HEIGHTMAP_H 