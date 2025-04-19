#include "ObjLoader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#include <QMap>

ObjModel ObjLoader::loadObj(const QString& filepath) {
    ObjModel model;
    QFile file(filepath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open OBJ file:" << filepath;
        return model;
    }
    
    QTextStream in(&file);
    
    QVector<QVector3D> positions;
    QVector<QVector2D> texCoords;
    QVector<QVector3D> normals;
    QMap<QString, int> vertexMap;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        if (line.isEmpty() || line.startsWith('#'))
            continue;
        
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        if (parts.isEmpty())
            continue;
        
        QString type = parts[0];
        
        if (type == "v" && parts.size() >= 4) {
            // Vertex position
            float x = parts[1].toFloat();
            float y = parts[2].toFloat();
            float z = parts[3].toFloat();
            positions.append(QVector3D(x, y, z));
        }
        else if (type == "vt" && parts.size() >= 3) {
            // Texture coordinate
            float u = parts[1].toFloat();
            float v = parts[2].toFloat();
            texCoords.append(QVector2D(u, v));
        }
        else if (type == "vn" && parts.size() >= 4) {
            // Normal vector
            float nx = parts[1].toFloat();
            float ny = parts[2].toFloat();
            float nz = parts[3].toFloat();
            normals.append(QVector3D(nx, ny, nz));
        }
        else if (type == "f" && parts.size() >= 4) {
            // Face
            // Handle triangles and quads
            for (int i = 1; i <= 3; ++i) {
                QStringList indices = parts[i].split('/');
                QString vertexKey = parts[i];
                
                int idx = -1;
                if (vertexMap.contains(vertexKey)) {
                    idx = vertexMap[vertexKey];
                } else {
                    // Create a new vertex
                    ObjVertex vertex;
                    
                    // OBJ indices are 1-based
                    int posIndex = indices[0].toInt() - 1;
                    if (posIndex >= 0 && posIndex < positions.size())
                        vertex.position = positions[posIndex];
                    
                    if (indices.size() > 1 && !indices[1].isEmpty()) {
                        int texIndex = indices[1].toInt() - 1;
                        if (texIndex >= 0 && texIndex < texCoords.size())
                            vertex.texCoord = texCoords[texIndex];
                    }
                    
                    if (indices.size() > 2 && !indices[2].isEmpty()) {
                        int normIndex = indices[2].toInt() - 1;
                        if (normIndex >= 0 && normIndex < normals.size())
                            vertex.normal = normals[normIndex];
                    }
                    
                    idx = model.vertices.size();
                    model.vertices.append(vertex);
                    vertexMap[vertexKey] = idx;
                }
                
                model.indices.append(idx);
            }
            
            // Handle quads by adding another triangle
            if (parts.size() >= 5) {
                QStringList indices1 = parts[1].split('/');
                QStringList indices3 = parts[3].split('/');
                QStringList indices4 = parts[4].split('/');
                
                // Add vertices for indices 1, 3, and 4 to form the second triangle
                for (int i = 0; i < 3; ++i) {
                    QString vertexKey;
                    QStringList indices;
                    
                    if (i == 0) {
                        vertexKey = parts[1];
                        indices = indices1;
                    } else if (i == 1) {
                        vertexKey = parts[3];
                        indices = indices3;
                    } else {
                        vertexKey = parts[4];
                        indices = indices4;
                    }
                    
                    int idx = -1;
                    if (vertexMap.contains(vertexKey)) {
                        idx = vertexMap[vertexKey];
                    } else {
                        // Create a new vertex
                        ObjVertex vertex;
                        
                        // OBJ indices are 1-based
                        int posIndex = indices[0].toInt() - 1;
                        if (posIndex >= 0 && posIndex < positions.size())
                            vertex.position = positions[posIndex];
                        
                        if (indices.size() > 1 && !indices[1].isEmpty()) {
                            int texIndex = indices[1].toInt() - 1;
                            if (texIndex >= 0 && texIndex < texCoords.size())
                                vertex.texCoord = texCoords[texIndex];
                        }
                        
                        if (indices.size() > 2 && !indices[2].isEmpty()) {
                            int normIndex = indices[2].toInt() - 1;
                            if (normIndex >= 0 && normIndex < normals.size())
                                vertex.normal = normals[normIndex];
                        }
                        
                        idx = model.vertices.size();
                        model.vertices.append(vertex);
                        vertexMap[vertexKey] = idx;
                    }
                    
                    model.indices.append(idx);
                }
            }
        }
    }
    
    file.close();
    
    // Calculate normals if none provided
    if (normals.isEmpty()) {
        calculateNormals(model);
    }
    
    qDebug() << "Loaded OBJ file" << filepath
             << "with" << model.vertices.size() << "vertices and"
             << model.indices.size() << "indices";
    
    return model;
}

void ObjLoader::calculateNormals(ObjModel& model) {
    // Initialize all normals to zero
    for (auto& vertex : model.vertices) {
        vertex.normal = QVector3D(0, 0, 0);
    }
    
    // For each triangle, calculate its normal and add it to each vertex
    for (int i = 0; i < model.indices.size(); i += 3) {
        if (i + 2 < model.indices.size()) {
            uint32_t idx1 = model.indices[i];
            uint32_t idx2 = model.indices[i + 1];
            uint32_t idx3 = model.indices[i + 2];
            
            if (idx1 < (uint32_t)model.vertices.size() && 
                idx2 < (uint32_t)model.vertices.size() && 
                idx3 < (uint32_t)model.vertices.size()) {
                
                QVector3D& v1 = model.vertices[idx1].position;
                QVector3D& v2 = model.vertices[idx2].position;
                QVector3D& v3 = model.vertices[idx3].position;
                
                QVector3D edge1 = v2 - v1;
                QVector3D edge2 = v3 - v1;
                QVector3D normal = QVector3D::crossProduct(edge1, edge2).normalized();
                
                model.vertices[idx1].normal += normal;
                model.vertices[idx2].normal += normal;
                model.vertices[idx3].normal += normal;
            }
        }
    }
    
    // Normalize the accumulated normals
    for (auto& vertex : model.vertices) {
        if (!vertex.normal.isNull()) {
            vertex.normal.normalize();
        } else {
            vertex.normal = QVector3D(0, 1, 0); // Default normal pointing up
        }
    }
}

bool ObjLoader::loadObjFile(const QString& filepath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
    // Load the model using the new method
    ObjModel model = loadObj(filepath);
    
    if (model.vertices.isEmpty()) {
        return false;
    }
    
    // Convert ObjModel to old format
    vertices.clear();
    indices.clear();
    
    // Create mapping from new vertex index to old vertex index
    QMap<int, int> vertexMap;
    
    // First pass: create unique vertices
    for (int i = 0; i < model.indices.size(); i++) {
        uint32_t idx = model.indices[i];
        
        if (!vertexMap.contains(idx)) {
            Vertex v;
            const ObjVertex& objVertex = model.vertices[idx];
            
            // Position
            v.x = objVertex.position.x();
            v.y = objVertex.position.y();
            v.z = objVertex.position.z();
            
            // Normal
            v.nx = objVertex.normal.x();
            v.ny = objVertex.normal.y();
            v.nz = objVertex.normal.z();
            
            // Texture coordinates
            v.u = objVertex.texCoord.x();
            v.v = objVertex.texCoord.y();
            
            vertexMap[idx] = vertices.size();
            vertices.push_back(v);
        }
        
        indices.push_back(vertexMap[idx]);
    }
    
    qDebug() << "ObjLoader::loadObjFile - Converted from new format to old format:"
             << model.vertices.size() << "original vertices,"
             << vertices.size() << "unique vertices,"
             << indices.size() << "indices";
    
    return !vertices.empty() && !indices.empty();
} 