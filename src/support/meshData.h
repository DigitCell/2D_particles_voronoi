#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <exception>
//#include "log.h"

struct TriangleVertices {
    uint one;
    uint two;
    uint three;
    TriangleVertices(){};
    TriangleVertices ( int one,int two, int three ) : one(one), two(two), three(three){};
};

class MeshData {
    public:
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec4> colors;
        std::vector<TriangleVertices> triangelVertices;

        std::vector<uint> lineIndices;

        int numberVertices() { return vertices.size();};
        int numberIndices()  { return 3*triangelVertices.size();};

        void recalculateVertices() {

            colors.clear();
            std::vector<glm::vec3> vertices_new;
            std::vector<TriangleVertices> triangelVertices_new;

            int vertexCounter=0;
            for (auto & v : triangelVertices) {
                TriangleVertices tempFace;
                vertices_new.push_back(vertices[v.one]);
                tempFace.one=vertexCounter;
                vertexCounter++;
                vertices_new.push_back(vertices[v.two]);
                tempFace.two=vertexCounter;
                vertexCounter++;
                vertices_new.push_back(vertices[v.three]);
                tempFace.three=vertexCounter;
                vertexCounter++;

                triangelVertices_new.push_back(tempFace);

                float deltaColorRed=((float)rand() / (RAND_MAX))/2.0f;
                float deltaColorGreen=((float)rand() / (RAND_MAX))/2.0f;
                float deltaColorBlue= ((float)rand() / (RAND_MAX))/2.0f;

                colors.push_back(glm::vec4(0.1f,0.2f+deltaColorGreen,0.3f+deltaColorBlue,1.0f));
                colors.push_back(glm::vec4(0.1f,0.2f+deltaColorGreen,0.3f+deltaColorBlue,1.0f));
                colors.push_back(glm::vec4(0.1f,0.2f+deltaColorGreen,0.3f+deltaColorBlue,1.0f));
            }

            vertices=vertices_new;
            triangelVertices=triangelVertices_new;

        };

        void recalculateVertices(const std::shared_ptr<MeshData> ptrMeshData) {

            colors.clear();
            std::vector<glm::vec3> vertices_new;
            std::vector<TriangleVertices> triangelVertices_new;

            int vertexCounter=0;
            for (auto & v : triangelVertices) {

                TriangleVertices tempFace;
                vertices_new.push_back(ptrMeshData->vertices[v.one]);
                tempFace.one=vertexCounter;
                vertexCounter++;
                vertices_new.push_back(ptrMeshData->vertices[v.two]);
                tempFace.two=vertexCounter;
                vertexCounter++;
                vertices_new.push_back(ptrMeshData->vertices[v.three]);
                tempFace.three=vertexCounter;
                vertexCounter++;

                triangelVertices_new.push_back(tempFace);

                float deltaColorRed=((float)rand() / (RAND_MAX))/2.0f;
                float deltaColorGreen=((float)rand() / (RAND_MAX))/2.0f;
                float deltaColorBlue= ((float)rand() / (RAND_MAX))/2.0f;

                colors.push_back(ptrMeshData->colors[v.one]);
                colors.push_back(ptrMeshData->colors[v.two]);
                colors.push_back(ptrMeshData->colors[v.three]);
            }

            vertices=vertices_new;
            triangelVertices=triangelVertices_new;

        };

        void addVertices(MeshData& newMeshData) {

            int vertexCounter=numberIndices();

            for (auto & v : newMeshData.triangelVertices) {

                TriangleVertices tempFace;
                vertices.push_back(newMeshData.vertices[v.one]);
                tempFace.one= vertexCounter;
                vertexCounter++;
                vertices.push_back(newMeshData.vertices[v.two]);
                tempFace.two=vertexCounter;
                vertexCounter++;
                vertices.push_back(newMeshData.vertices[v.three]);
                tempFace.three=vertexCounter;
                vertexCounter++;

                triangelVertices.push_back(tempFace);

                float deltaColorRed=((float)rand() / (RAND_MAX))/2.0f;
                float deltaColorGreen=((float)rand() / (RAND_MAX))/2.0f;
                float deltaColorBlue= ((float)rand() / (RAND_MAX))/2.0f;

                glm::vec4 newColor=glm::vec4(0.45f+deltaColorRed,0.2f,0.3f+deltaColorBlue,1.0f);

                colors.push_back(newColor);
                colors.push_back(newColor);
                colors.push_back(newColor);
            }
        };

        void recalculateLineIndices() {

            lineIndices.clear();
            int vertexCounter=0;
            for (auto & v : triangelVertices)
            {
                lineIndices.push_back(v.one);
                lineIndices.push_back(v.two);

                lineIndices.push_back(v.two);
                lineIndices.push_back(v.three);

                lineIndices.push_back(v.three);
                lineIndices.push_back(v.one);
            }
        };

        void ClearAll()
        {
            vertices.clear();
            normals.clear();
            triangelVertices.clear();
        }

        void removeUnusedVertices ();
        void transformMesh(std::function<glm::vec3 (glm::vec3 &)> transformVerticeFn);
        
};
