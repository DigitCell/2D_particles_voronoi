#include "Constants.hpp"

#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include <glm/vec3.hpp>
#include "vector"
#include "iostream"
#include "tuple"
#include <opencv2/opencv.hpp>

#pragma once

struct TEventData {
  float wheelDelta;
  float mouseX;
  float mouseY;
  bool bMouseMove;
  bool bRotatePressed;
  bool bTranslatePressed;
  bool bSpacePressed;
};

struct Sim_parameters{
    bool runSimulation=false;
    bool restartSimulation=false;
    bool nextStep=false;
    bool drawMesh=true;
    bool generateImage=false;

    int tick=0;
    float dt=0.35f;
    float maxVelocity=1.25f;
    float radius=11.0f;
    float noiseCoeff=1.0f;
    float stiffnessCoeff=0.035;
    float dampingCoeff=0.2;
};



struct PointData
{
    glm::vec2 coord;
    glm::vec2 velocity;
    glm::vec2 force;
    glm::vec2 headTail;
    glm::vec2 prevNext;
    glm::vec2 radius;

};

struct NeightbData
{
    glm::vec2 dist;

};

struct VoronoiData
{
    glm::vec2 index;
    glm::vec2 coord;
    glm::ivec4 nb;
    glm::vec2 dist;
};


struct Color{
    float r = 1.f, g = 1.f, b = 1.f;
};


struct Box {
    float x, y, z;
};


struct Image_statistics{

    // statistics
    uint32_t acc_r = 0, acc_g = 0, acc_b = 0;
    uint8_t  min_r = 255, min_g = 255, min_b = 255;
    uint8_t  max_r = 0, max_g = 0, max_b = 0;

    uint32_t white = 0,
             black = 0;

    std::string function;
    int function_seed;
};


class Pos
{
    public:

    inline Pos() : x(0), y(0) {}
    inline Pos(int _x) :  x(_x), y(_x) {}
    inline Pos(int _x, int _y) : x(_x), y(_y) {}
    Pos(const int* p) : x(p[0]), y(p[1]) {}


    explicit Pos(const Pos& v) : x(v.x), y(v.y) {}


    operator int* () { return &x; }
    operator const int* () const { return &x; }

    void Set(int x_, int y_) { x = x_; y = y_; }

    inline bool operator == (const Pos& other) const { return x == other.x && y == other.y; }
    inline bool operator != (const Pos& other) const { return x != other.x || y != other.y; }

    inline Pos operator +(const Pos& v) const { Pos r(*this); r+=v; return r; }
    inline Pos operator -(const Pos& v) const { Pos r(*this); r-=v; return r; }

    inline Pos& operator +=(const Pos& v) { x+=v.x, y+=v.y; return *this;}
    inline Pos& operator -=(const Pos& v) { x-=v.x; y-=v.y; return *this;}

    int x;
    int y;

};

struct MapMat
{
   bool use=true;
   bool f_use=false;
   int index=0;
   int x=0;
   int y=0;
   int newX=0;
   int newY=0;

   cv::Mat quard;
   std::vector<cv::Mat> borders;
   cv::Mat leftBorder;
   cv::Mat rightBorder;
   cv::Mat upBorder;
   cv::Mat downBorder;
};


class GridMat
{
    public:
        GridMat(){}
        GridMat(int width, int height) : width_(width), height_(height), cells(width * height) {}

        void resize(int width, int height){ width_=width; height_=height; cells.resize(width * height);}
        MapMat& at(int row, int column) { return cells[index(row, column)]; }
        MapMat& at(Pos pos) { return cells[indexPos(pos)]; }
        std::vector<MapMat> cells;

    private:
        int width_;
        int height_;
        inline int index(int x, int y){
            return int(y * width_ + x);}

        inline int indexPos(Pos pos){
            return int(pos.y * width_ + pos.x);}
};
