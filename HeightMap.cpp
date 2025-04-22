#include "HeightMap.h"
#include "stb_image.h"
#include <QDebug>
#include <cmath>

HeightMap::HeightMap()
{
    // Constructor initializes an empty HeightMap
    // Actual terrain generation happens in makeTerrain methods
}

void HeightMap::makeTerrain(std::string heightMapImage)
{
    // Load the heightmap image
    // Using stb_image to load the image
    stbi_uc* pixelData = stbi_load(heightMapImage.c_str(), &mWidth, &mHeight, &mChannels, STBI_rgb_alpha);
    if (pixelData == nullptr)
    {
        qDebug() << "Failed to load heightmap image!";
        return;
    }
    // Make the terrain from the pixel data
    makeTerrain(pixelData, mWidth, mHeight);
    stbi_image_free(pixelData);
}

void HeightMap::makeTerrain(unsigned char* textureData, int widthIn, int heightIn)
{
    // Clear any existing vertices and indices
    mVertices.clear();
    mIndices.clear();

    // Default normal pointing straight up - should be calculated correctly for lights to work!!!
    float normal[3]{0.f, 1.f, 0.f};

    // Store the dimensions
    mWidth = widthIn;
    mHeight = heightIn;

    // Getting the scale of the heightmap
    // Using depth as the name of texture height, to not confuse with terrain height
    unsigned short width = widthIn;       // Width == x-axis
    unsigned short depth = heightIn;      // Depth == z-axis

    // Temp variables for creating the mesh
    // Adding offset so the middle of the terrain will be in World origo
    float vertexXStart{ 0.f - width * mHorizontalSpacing / 2.f };
    float vertexZStart{ 0.f + depth * mHorizontalSpacing / 2.f };

    // Loop to make the mesh from the values read from the heightmap (textureData)
    // Double for-loop to make the depth and the width of the terrain in one go
    for(int d{0}; d < depth; ++d)       // depth loop
    {
        for(int w{0}; w < width; ++w)   // width loop
        {
            // Heightmap image is actually stored as an one dimensional array
            // so calculating the correct index for column and row
            // and scale it according to variables
            // Calculate the correct index for the R value of each pixel
            int index = (w + d * width) * 4; // Each pixel has 4 bytes (RGBA)
            float heightFromBitmap = static_cast<float>(textureData[index]) * mHeightSpacing + mHeightPlacement;

            // Create a vertex with position, normal and texture coordinates
            ObjLoader::Vertex v;
            
            // Position
            v.x = vertexXStart + (w * mHorizontalSpacing);
            v.y = heightFromBitmap;
            v.z = vertexZStart - (d * mHorizontalSpacing);
            
            // Normal
            v.nx = normal[0];
            v.ny = normal[1];
            v.nz = normal[2];
            
            // Texture coordinates 
            v.u = w / (width - 1.f);
            v.v = d / (depth - 1.f);
            
            mVertices.push_back(v);
        }
    }

    // The mesh(grid) is drawn in quads with diagonals from lower left to upper right
    //          _ _
    //         |/|/|
    //          - -
    //         |/|/|
    //          - -
    // Making the indices for this mesh:
    for(int d{0}; d < depth-1; ++d)        // depth - 1 because we draw the last quad from depth - 1 and in negative z direction
    {
        for(int w{0}; w < width-1; ++w)    // width - 1 because we draw the last quad from width - 1 and in positive x direction
        {
            // Indices for one quad (two triangles):
            mIndices.push_back(w + d * width);               // 0 + 0 * mWidth               = 0
            mIndices.push_back(w + d * width + width + 1);   // 0 + 0 * mWidth + mWidth + 1  = mWidth + 1
            mIndices.push_back(w + d * width + width);       // 0 + 0 * mWidth + mWidth      = mWidth
            mIndices.push_back(w + d * width);               // 0 + 0 * mWidth               = 0
            mIndices.push_back(w + d * width + 1);           // 0 + 0 * mWidth + 1           = 1
            mIndices.push_back(w + d * width + width + 1);   // 0 + 0 * mWidth + mWidth + 1  = mWidth + 1
        }
    }

    qDebug() << "Created heightmap with" << mVertices.size() << "vertices and" << mIndices.size() << "indices";
}

float HeightMap::getHeightAt(float x, float z)
{
    // This is a simplified implementation to find the height at a specific position
    // A more accurate implementation would use barycentric coordinates (as mentioned in step 5)
    
    // Convert world coordinates to heightmap grid coordinates
    float gridX = (x - (0.f - mWidth * mHorizontalSpacing / 2.f)) / mHorizontalSpacing;
    float gridZ = ((0.f + mHeight * mHorizontalSpacing / 2.f) - z) / mHorizontalSpacing;
    
    // Check if we're outside the heightmap boundaries
    if (gridX < 0 || gridX >= mWidth - 1 || gridZ < 0 || gridZ >= mHeight - 1)
        return 0.0f; // Return 0 if outside the heightmap
    
    // Find the grid cell that contains the point
    int x0 = static_cast<int>(gridX);
    int z0 = static_cast<int>(gridZ);
    
    // Calculate the fractional parts
    float fracX = gridX - x0;
    float fracZ = gridZ - z0;
    
    // Get the four corner heights
    float h00 = mVertices[x0 + z0 * mWidth].y;
    float h10 = mVertices[(x0 + 1) + z0 * mWidth].y;
    float h01 = mVertices[x0 + (z0 + 1) * mWidth].y;
    float h11 = mVertices[(x0 + 1) + (z0 + 1) * mWidth].y;
    
    // Bilinear interpolation
    float h0 = h00 * (1.0f - fracX) + h10 * fracX;
    float h1 = h01 * (1.0f - fracX) + h11 * fracX;
    float height = h0 * (1.0f - fracZ) + h1 * fracZ;
    
    return height;
} 