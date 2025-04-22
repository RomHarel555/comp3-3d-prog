#pragma once

#include <QVector2D>
#include <QVector3D>
#include <vector>
#include "ObjLoader.h"

// Structure to represent a triangle in 3D space for heightmap terrain following
struct Triangle {
    QVector3D v0, v1, v2;  // Positions of the three vertices
};

// Heightmap terrain following implementation using barycentric coordinates
class TerrainFollowing {
public:
    // Find the triangle in the heightmap that contains the point (x,z)
    static Triangle findTriangleAtPosition(float x, float z, 
                                           const std::vector<ObjLoader::Vertex>& heightmapVertices,
                                           const std::vector<uint32_t>& heightmapIndices) {
        // Create a point with the player's x,z position (y doesn't matter for the search)
        QVector3D playerPos(x, 0.0f, z);
        
        // Find the triangle that contains this point
        for (size_t i = 0; i < heightmapIndices.size(); i += 3) {
            // Get the three vertices of this triangle
            const ObjLoader::Vertex& v0 = heightmapVertices[heightmapIndices[i]];
            const ObjLoader::Vertex& v1 = heightmapVertices[heightmapIndices[i+1]];
            const ObjLoader::Vertex& v2 = heightmapVertices[heightmapIndices[i+2]];
            
            // Create QVector3D objects for the triangle vertices
            QVector3D vert0(v0.x, v0.y, v0.z);
            QVector3D vert1(v1.x, v1.y, v1.z);
            QVector3D vert2(v2.x, v2.y, v2.z);
            
            // Check if the point is inside this triangle (only using X and Z coordinates)
            if (isPointInTriangle2D(playerPos, vert0, vert1, vert2)) {
                // Return the triangle if the point is inside
                return Triangle{ vert0, vert1, vert2 };
            }
        }
        
        // If no triangle is found, return a default triangle at y=0
        // This is a fallback case if the player is outside the heightmap
        return Triangle{ 
            QVector3D(x - 1.0f, 0.0f, z - 1.0f),
            QVector3D(x + 1.0f, 0.0f, z - 1.0f),
            QVector3D(x, 0.0f, z + 1.0f)
        };
    }

    // Check if a point is inside a triangle (only considering X and Z coordinates)
    static bool isPointInTriangle2D(const QVector3D& p, const QVector3D& v0, const QVector3D& v1, const QVector3D& v2) {
        // Using 2D barycentric coordinates to check if a point is inside a triangle
        // Ignoring Y component (height) and only using X and Z
        
        // Compute vectors
        QVector2D p2d(p.x(), p.z());
        QVector2D v0_2d(v0.x(), v0.z());
        QVector2D v1_2d(v1.x(), v1.z());
        QVector2D v2_2d(v2.x(), v2.z());
        
        // Compute barycentric coordinates
        QVector2D v0v1 = v1_2d - v0_2d;
        QVector2D v0v2 = v2_2d - v0_2d;
        QVector2D v0p = p2d - v0_2d;
        
        // Compute dot products
        float d00 = QVector2D::dotProduct(v0v1, v0v1);
        float d01 = QVector2D::dotProduct(v0v1, v0v2);
        float d11 = QVector2D::dotProduct(v0v2, v0v2);
        float d20 = QVector2D::dotProduct(v0p, v0v1);
        float d21 = QVector2D::dotProduct(v0p, v0v2);
        
        // Compute barycentric coordinates
        float denom = d00 * d11 - d01 * d01;
        if (std::abs(denom) < 1e-6f) // Avoid division by zero for degenerate triangles
            return false;
            
        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;
        
        // Check if point is in triangle (allowing for small floating-point errors)
        static const float EPSILON = 1e-6f;
        return (u >= -EPSILON) && (v >= -EPSILON) && (w >= -EPSILON) && (u + v + w <= 1.0f + EPSILON);
    }

    // Calculate barycentric coordinates for a point in a triangle
    static QVector3D calculateBarycentric(const QVector3D& p, const QVector3D& a, const QVector3D& b, const QVector3D& c) {
        // Calculate the barycentric coordinates of point p in triangle (a,b,c)
        // Using only X and Z components for the calculation
        
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

    // Get the height at a specific position on the terrain using barycentric coordinates
    static float getHeightAtPosition(float x, float z, 
                                     const std::vector<ObjLoader::Vertex>& heightmapVertices,
                                     const std::vector<uint32_t>& heightmapIndices) {
        // Find the triangle the player is on
        Triangle tri = findTriangleAtPosition(x, z, heightmapVertices, heightmapIndices);
        
        // Calculate barycentric coordinates for the triangle
        QVector3D bary = calculateBarycentric(QVector3D(x, 0.0f, z), tri.v0, tri.v1, tri.v2);
        
        // Use barycentric coordinates to interpolate height
        float height = bary.x() * tri.v0.y() + bary.y() * tri.v1.y() + bary.z() * tri.v2.y();
        
        // Apply some bounds checking (optional)
        if (height < -20.0f || height > 20.0f) {
            qDebug("Warning: Calculated unusual height: %f at position (%f, %f) - Clamping to safe range", 
                   height, x, z);
            height = std::max(-20.0f, std::min(height, 20.0f));
        }
        
        return height;
    }
}; 