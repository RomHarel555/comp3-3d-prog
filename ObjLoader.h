#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <QString>
#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <vector>

struct ObjVertex {
    QVector3D position;
    QVector3D normal;
    QVector2D texCoord;
};

struct ObjModel {
    QVector<ObjVertex> vertices;
    QVector<uint32_t> indices;
};

class ObjLoader {
public:
    // New interface
    static ObjModel loadObj(const QString& filepath);
    
    // Compatibility with old interface
    struct Vertex {
        float x, y, z;     // Position
        float nx, ny, nz;  // Normal
        float u, v;        // Texture coordinates
    };
    
    static bool loadObjFile(
        const QString& filepath,
        std::vector<Vertex>& vertices,
        std::vector<uint32_t>& indices
    );
    
private:
    static void calculateNormals(ObjModel& model);
};

#endif // OBJLOADER_H 