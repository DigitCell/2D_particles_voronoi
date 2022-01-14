#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "Constants.hpp"
#include "support/texture.hpp"
#include "support/ComputeProcess.h"

#include "support/texture.hpp"
#include <opencv2/opencv.hpp>

#include "opencv2/img_hash.hpp"

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "support/tinycolormap.hpp"

using namespace std;

class Solver:  ComputeProcess
{

public:
    Solver();

    int Tick;
    Texture function_image;
    std::vector<uint8_t> colors;
    vector<glm::vec4> colorStateMap;

    int pointsNumber=particles_init;
    PointData *points;

    bool ptrInit=true;
    float *colorVert;
    float *pointPos;
    VoronoiData *pointDist;
    int worldElementsAmount;

    struct DataProcessor
    {

        struct Buffers
        {
            Buffer pointData;
            Buffer pointPos;
            Buffer mapData;
            Buffer mapSize;
            Buffer mapSizeFloat;
            Buffer ColorMapData;

            Buffer neightb_size;
            Buffer neightb;
            Buffer neightbSort;
            Buffer neightbSortSpec;
            Buffer neightbInfo;

            Buffer distMapData;

            struct {
                GLuint id;     //  Target image texture
                unsigned int width;
                unsigned int heights;
                unsigned int resolution;
            } writeToTexture;

            struct {
                GLuint id;     //  Target image texture
                unsigned int width;
                unsigned int heights;
                unsigned int resolution;
            } distMapTexture;


        } buffers;

        struct Shaders
        {
            ComputeProgram pointsPhysics;
            ComputeProgram mapUpdate;
            ComputeProgram mapClear;
            ComputeProgram neightbUpdate;
            ComputeProgram neightbSort;

            ComputeProgram writeToTexture;
            ComputeProgram distMap;
            ComputeProgram distMapClear;
            ComputeProgram distMapDraw;

            int numberOffsets=1;
            int offsetSize=1;

        } shaders;


        /*
        struct Volumes
        {
            Volume cubeGrid;
            Volume cubeGridNeighbors;
            int stepGridNeighbors=1;
            int maxNumberdNeighbors=1;
        } volumes;

        struct {
            struct {
                GLuint img;     //  Target image texture
                unsigned int width;
                unsigned int heights;
                unsigned int resolution;
            } imageTexture;
        } buffers;
*/


    } dataProcessor;

    glm::vec2 worldSize;
    glm::ivec2 mapSize;
    glm::vec2 mapCellSize;
    int mapElementsAmount;

    float radius;
    float voronoi_scaleradius=1.0f;

    void createProgram();
    void createBuffers();
    void deleteProgram();
    void deleteBuffers();

    void updateDispatchParams();
    void resizeBoidBuffer();


    bool setup();
    void importDataToBUffers();

    void update(Sim_parameters& params);

    bool hasBuffers = false, hasProgram = false;

    ~Solver();

    bool numBoidsChanged = true, numRayDirsChanged = true;

    inline glm::vec3 unitRandom()
    {
        float phi = (float)rand()/(float)RAND_MAX * glm::pi<float>() * 2.f;
        float costheta = (float)rand()/(float)RAND_MAX * 2.f - 1.f;

        float theta = acos(costheta);
        float x = sin(theta) * cos(phi);
        float y = sin(theta) * sin(phi);
        float z = cos(theta);

        return glm::vec3(x,y,z);
    }

    void generate_image();




};

#endif // SOLVER_HPP
