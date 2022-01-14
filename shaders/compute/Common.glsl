#define PointData_BP     0
#define PointPos_BP      1
#define ColorList_BP     2
#define MapData_BP       3
#define MapSize_BP       4
#define MapSizeFloat_BP  5
#define Neightb_BP       6
#define NeightbSize_BP   7
#define NeightbSort_BP   8
#define NeightbInfo_BP   9
#define NeightSpec_BP   10
#define DistMapData_BP  11

//precision highp float;
//precision highp int;

layout (local_size_variable) in;

struct PointData
{
    vec2 coord;
    vec2 velocity;
    vec2 force;
    vec2 headTail;
    vec2 prevNext;
    vec2 radius;
};


struct NeightbData
{
   vec2 dist;
};

struct VoronoiData
{
    vec2 index;
    vec2 coord;
    ivec4 nb;
    vec2 dist;
};

layout (std430, binding = PointData_BP ) buffer PointDataBuffer
{
    PointData pointData[];
};

layout (std430, binding = PointPos_BP  ) buffer PointPosBuffer
{
    float pointPos[];
};

layout (std430, binding = MapData_BP  ) buffer MapDataBuffer
{
    int mapData[];
};

layout (std430, binding = MapSize_BP  ) buffer MapSizeBuffer
{
    int mapIndexSize[];
};

layout (std430, binding = MapSizeFloat_BP  ) buffer MapSizeFloatBuffer
{
    float mapIndexSizeFloat[];
};

layout (std430, binding = Neightb_BP  ) buffer NeightbBuffer
{
    int neightb[];
};

layout (std430, binding = NeightbSize_BP  ) buffer NeightbSizeBuffer
{
    int neightbSize[];
};

layout (std430, binding = NeightbSort_BP  ) buffer NeightbSortBuffer
{
    int neightbSort[];
};

layout (std430, binding = NeightbInfo_BP  ) buffer NeightbInfoBuffer
{
    NeightbData neightbInfo[];
};

layout (std430, binding = DistMapData_BP ) buffer DistMapDataBuffer
{
    VoronoiData distMapData[];
};

//Iq palettes https://www.shadertoy.com/view/ll2GD3
vec3 pal( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d )
{
    return a + b*cos( 6.28318*(c*t+d) );
}
