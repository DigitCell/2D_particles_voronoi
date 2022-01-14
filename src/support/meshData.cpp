#include "meshData.h"

using namespace glm;

// function to remove all vertices that are not used by any polygon
void MeshData::removeUnusedVertices() {
    
    int verticesCounter=1;
    
    std::vector<int> lookup(vertices.size() + 1,-1);
    std::vector<vec3> filteredVertices;
     
    auto renameFnc = [&](int trianglePointIndx)-> int{
        int newIndx;
        if (lookup[trianglePointIndx] < 0) {
            lookup[trianglePointIndx] = verticesCounter;
            filteredVertices.push_back(vertices[trianglePointIndx-1]);        
            newIndx =verticesCounter; 
            verticesCounter++;   
        } else {
            newIndx= lookup[trianglePointIndx];
        }
        if ( newIndx > vertices.size() + 1 ) {
           // LOG_CRITICAL_THROW("New index cannot be bigger than original!");
        }
        return newIndx;
    };    

    for (auto & v : triangelVertices) {        
        v.one = renameFnc(v.one);
        v.two = renameFnc(v.two);
        v.three = renameFnc(v.three);
    }
    vertices = filteredVertices;
}

void MeshData::transformMesh(std::function<glm::vec3 (glm::vec3 &)> transformVerticeFn) {
    for ( auto & v : vertices ) {
            v = transformVerticeFn(v);
    }
}
