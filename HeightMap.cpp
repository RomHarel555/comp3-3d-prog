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
    
    // Get the four corner heights and positions
    int idx00 = x0 + z0 * mWidth;
    int idx10 = (x0 + 1) + z0 * mWidth;
    int idx01 = x0 + (z0 + 1) * mWidth;
    int idx11 = (x0 + 1) + (z0 + 1) * mWidth;
    
    // Make sure indices are valid
    if (idx00 >= mVertices.size() || idx10 >= mVertices.size() || 
        idx01 >= mVertices.size() || idx11 >= mVertices.size()) {
        qDebug() << "Invalid heightmap indices at" << x << "," << z;
        return 0.0f;
    }
    
    // Get the vertex positions
    QVector3D v00(mVertices[idx00].x, mVertices[idx00].y, mVertices[idx00].z);
    QVector3D v10(mVertices[idx10].x, mVertices[idx10].y, mVertices[idx10].z);
    QVector3D v01(mVertices[idx01].x, mVertices[idx01].y, mVertices[idx01].z);
    QVector3D v11(mVertices[idx11].x, mVertices[idx11].y, mVertices[idx11].z);
    
    // Create a point for the player position
    QVector3D p(x, 0.0f, z);
    
    // Determine which triangle in the quad we're on and calculate barycentric coordinates
    float height;
    if (fracX + fracZ <= 1.0f) {
        // Top-left triangle (v00, v10, v01)
        // Calculate barycentric coordinates
        QVector3D bary = calculateBarycentric(p, v00, v10, v01);
        
        // Use barycentric coordinates to interpolate height
        height = bary.x() * v00.y() + bary.y() * v10.y() + bary.z() * v01.y();
    } else {
        // Bottom-right triangle (v11, v01, v10)
        // Calculate barycentric coordinates
        QVector3D bary = calculateBarycentric(p, v11, v01, v10);
        
        // Use barycentric coordinates to interpolate height
        height = bary.x() * v11.y() + bary.y() * v01.y() + bary.z() * v10.y();
    }
    
    return height;
}

// Helper function to calculate barycentric coordinates
QVector3D HeightMap::calculateBarycentric(const QVector3D& p, const QVector3D& a, const QVector3D& b, const QVector3D& c)
{
    // Calculate the barycentric coordinates of point p in triangle (a,b,c)
    // Using only X and Z components for the calculation (ignoring height)
    
    // Compute vectors for the 2D plane (ignoring height)
    QVector2D p2d(p.x(), p.z());
    QVector2D a2d(a.x(), a.z());
    QVector2D b2d(b.x(), b.z());
    QVector2D c2d(c.x(), c.z());
    
    // Calculate vectors from point a to the other points
    QVector2D v0 = b2d - a2d;
    QVector2D v1 = c2d - a2d;
    QVector2D v2 = p2d - a2d;
    
    // Compute dot products
    float d00 = QVector2D::dotProduct(v0, v0);
    float d01 = QVector2D::dotProduct(v0, v1);
    float d11 = QVector2D::dotProduct(v1, v1);
    float d20 = QVector2D::dotProduct(v2, v0);
    float d21 = QVector2D::dotProduct(v2, v1);
    
    // Calculate barycentric coordinates
    float denom = d00 * d11 - d01 * d01;
    if (std::abs(denom) < 1e-6f) // Avoid division by zero for degenerate triangles
        return QVector3D(1.0f, 0.0f, 0.0f); // Return default value if triangle is degenerate
        
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;
    
    return QVector3D(u, v, w);
} 