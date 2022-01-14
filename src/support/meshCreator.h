#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#include <vector>

#include "meshData.h"
#include "pointTransformer.h"

#include "Icosphere.h"


class MeshCreator {
    public:
    static MeshData createMeshOfPlane  (double squareSizeOfPlane , glm::vec3 originPlane , glm::vec3 normalOfPlane) {
        glm::vec3 originUp = glm::vec3(0.0f,0.0f, 1.0f);
        
        PointTransformer transformer(originUp,normalOfPlane,originPlane);

        // create square 
        glm::vec3 leftTop = glm::vec3(-squareSizeOfPlane/2,+squareSizeOfPlane/2,0.0f);
        glm::vec3 rightTop = glm::vec3(+squareSizeOfPlane/2,+squareSizeOfPlane/2,0.0f);
        glm::vec3 rightBottom = glm::vec3(+squareSizeOfPlane/2,-squareSizeOfPlane/2,0.0f);
        glm::vec3 leftBottom = glm::vec3(-squareSizeOfPlane/2,-squareSizeOfPlane/2,0.0f);

        MeshData data;
        data.vertices.push_back(transformer.transformTargetToOrigin(leftTop));  // index 1
        data.vertices.push_back(transformer.transformTargetToOrigin(rightTop)); // index 2
        data.vertices.push_back(transformer.transformTargetToOrigin(rightBottom)); // index 3
        data.vertices.push_back(transformer.transformTargetToOrigin(leftBottom)); // index 4

        data.triangelVertices.push_back(TriangleVertices(1,2,4));
        data.triangelVertices.push_back(TriangleVertices(4,3,2));
        
        return data;
    }

    static MeshData createCube(float size) {
        glm::vec3 originUp = glm::vec3(0.0f,0.0f, 1.0f);


        const int VERTEX_COUNT = 24;
        const int INDEX_COUNT = 36;

           glm::vec3 VERTICES[VERTEX_COUNT] =
           {
               {-1.0f,  1.0f,  1.0f},
               { 1.0f,  1.0f,  1.0f},
               {-1.0f, -1.0f,  1.0f},
               { 1.0f, -1.0f,  1.0f},
               {-1.0f,  1.0f, -1.0f},
               { 1.0f,  1.0f, -1.0f},
               {-1.0f, -1.0f, -1.0f},
               { 1.0f, -1.0f, -1.0f},
               {-1.0f,  1.0f,  1.0f},
               { 1.0f,  1.0f,  1.0f},
               {-1.0f,  1.0f, -1.0f},
               { 1.0f,  1.0f, -1.0f},
               {-1.0f, -1.0f,  1.0f},
               { 1.0f, -1.0f,  1.0f},
               {-1.0f, -1.0f, -1.0f},
               { 1.0f, -1.0f, -1.0f},
               { 1.0f, -1.0f,  1.0f},
               { 1.0f,  1.0f,  1.0f},
               { 1.0f, -1.0f, -1.0f},
               { 1.0f,  1.0f, -1.0f},
               {-1.0f, -1.0f,  1.0f},
               {-1.0f,  1.0f,  1.0f},
               {-1.0f, -1.0f, -1.0f},
               {-1.0f,  1.0f, -1.0f}
           };

           glm::vec3 NORMALS[VERTEX_COUNT] =
           {
               { 0.0f,  0.0f,  1.0f},
               { 0.0f,  0.0f,  1.0f},
               { 0.0f,  0.0f,  1.0f},
               { 0.0f,  0.0f,  1.0f},
               { 0.0f,  0.0f, -1.0f},
               { 0.0f,  0.0f, -1.0f},
               { 0.0f,  0.0f, -1.0f},
               { 0.0f,  0.0f, -1.0f},
               { 0.0f,  1.0f,  0.0f},
               { 0.0f,  1.0f,  0.0f},
               { 0.0f,  1.0f,  0.0f},
               { 0.0f,  1.0f,  0.0f},
               { 0.0f, -1.0f,  0.0f},
               { 0.0f, -1.0f,  0.0f},
               { 0.0f, -1.0f,  0.0f},
               { 0.0f, -1.0f,  0.0f},
               { 1.0f,  0.0f,  0.0f},
               { 1.0f,  0.0f,  0.0f},
               { 1.0f,  0.0f,  0.0f},
               { 1.0f,  0.0f,  0.0f},
               {-1.0f,  0.0f,  0.0f},
               {-1.0f,  0.0f,  0.0f},
               {-1.0f,  0.0f,  0.0f},
               {-1.0f,  0.0f,  0.0f}
           };

           int INDICES[INDEX_COUNT] =
           {
                   0,  2,  1,
                   1,  2,  3,
                   4,  5,  6,
                   5,  7,  6,

                   8, 10,  9,
                   9, 10, 11,
                  12, 13, 14,
                  13, 15, 14,

                  16, 18, 17,
                  17, 18, 19,
                  20, 21, 22,
                  21, 23, 22,
           };

        MeshData data;

        for(int i=0; i<VERTEX_COUNT; i++)
        {
            data.vertices.push_back(VERTICES[i]);
            data.normals.push_back(NORMALS[i]);

            float deltaColorGreen=((float)rand() / (RAND_MAX))/2.0f;
            float deltaColorBlue= ((float)rand() / (RAND_MAX))/2.0f;
            data.colors.push_back(glm::vec4(0.1f,0.2f+deltaColorGreen,0.3f+deltaColorBlue,1.0f));
        }

        int numberFaces=INDEX_COUNT/3;

        for(int i=0; i<numberFaces; i++)
        {
            data.triangelVertices.push_back(TriangleVertices(INDICES[i*3+0], INDICES[i*3+1],INDICES[i*3+2]));

            float deltaColorRed=((float)rand() / (RAND_MAX))/2.0f;
            float deltaColorGreen=((float)rand() / (RAND_MAX))/2.0f;
            float deltaColorBlue= ((float)rand() / (RAND_MAX))/2.0f;
            data.colors[INDICES[i*3+0]]=glm::vec4(0.1f+deltaColorRed,0.2f+deltaColorGreen,0.3f+deltaColorBlue,1.0f);
            data.colors[INDICES[i*3+1]]=glm::vec4(0.1f+deltaColorRed,0.2f+deltaColorGreen,0.3f+deltaColorBlue,1.0f);
            data.colors[INDICES[i*3+2]]=glm::vec4(0.1f+deltaColorRed,0.2f+deltaColorGreen,0.3f+deltaColorBlue,1.0f);

        }

        return data;
    }


    static MeshData createSphere (float size) {

        unsigned int const num_component = 3u;
        int subdivision = 1;
        Icosphere sphere(size, subdivision, false);

        MeshData data;
        glm::vec3 originUp = glm::vec3(0.0f,0.0f, 1.0f);


        int VERTEX_COUNT = sphere.getVertexCount();
        int INDEX_COUNT = sphere.getIndexCount();


        for(int i=0; i<VERTEX_COUNT; i++)
        {
            data.vertices.push_back(glm::vec3(sphere.getVertices()[i*3+0]/2.f,
                                              sphere.getVertices()[i*3+1]/2.f,
                                              sphere.getVertices()[i*3+2]/2.f));
            data.normals.push_back(glm::vec3( sphere.getNormals()[i*3+0],
                                              sphere.getNormals()[i*3+1],
                                              sphere.getNormals()[i*3+2]));

            float deltaColorGreen=((float)rand() / (RAND_MAX))/2.0f;
            float deltaColorBlue= ((float)rand() / (RAND_MAX))/2.0f;
            data.colors.push_back(glm::vec4(0.1f,0.2f+deltaColorGreen,0.3f+deltaColorBlue,1.0f));
        }

        int numberFaces=INDEX_COUNT/3;

        for(int i=0; i<numberFaces; i++)
        {
            data.triangelVertices.push_back(TriangleVertices(sphere.getIndices()[i*3+0],
                                                             sphere.getIndices()[i*3+1],
                                                             sphere.getIndices()[i*3+2]));

            float deltaColorRed=((float)rand() / (RAND_MAX))/2.0f;
            float deltaColorGreen=((float)rand() / (RAND_MAX))/2.0f;
            float deltaColorBlue= ((float)rand() / (RAND_MAX))/2.0f;
            data.colors[sphere.getIndices()[i*3+0]]=glm::vec4(0.1f+deltaColorRed,0.2f+deltaColorGreen,0.3f+deltaColorBlue,1.0f);
            data.colors[sphere.getIndices()[i*3+1]]=glm::vec4(0.1f+deltaColorRed,0.2f+deltaColorGreen,0.3f+deltaColorBlue,1.0f);
            data.colors[sphere.getIndices()[i*3+2]]=glm::vec4(0.1f+deltaColorRed,0.2f+deltaColorGreen,0.3f+deltaColorBlue,1.0f);

        }


         return data;

    }

};
