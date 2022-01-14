#version 430

#extension GL_ARB_compute_variable_group_size : enable

#include "Common.glsl"

uniform int numPoints;
uniform float radius;
uniform vec2 worldSize;
uniform ivec2 mapSize;
uniform int mapDivider;

uniform vec2 mapCellSize;
uniform int maxNeightbSize;
uniform int maxCellSize;

//layout(rgba32f) uniform writeonly image2D writeToTexture;
layout(rgba32f) uniform writeonly image2D distinationTexture;

int GetIndex(int x, int y)
{
    return y*mapSize.x+x;
}

int GetIndexMap(int i, int x, int y)
{
    int sindex=y*mapSize.x+x;
    return sindex*maxCellSize+i;
}

ivec2 iW2xy(int id)
{
    return ivec2(id%int(mapSize.x ), id/int(mapSize.x ));
}
int GetNeightbIndex(int index, int neightb_index)
{
    return index*maxNeightbSize+neightb_index;
}

#define N ivec2(int(sqrt(numPoints)),int(sqrt(numPoints)))

ivec2 i2xy(int id)
{
    return ivec2(id%int( N ), id/int( N ));
}

int xy2i(ivec2 p)
{
    return p.x + p.y*int( N );
}

int GetTextureIndex(int x, int y)
{
    return y*ivec2(worldSize).x+x;
}

int GetWorldVec2Index(vec2 coord)
{
    return int(coord.y*worldSize.x+coord.x);
}

int GetWorldiVec2Index(ivec2 coord)
{
    return coord.y*int(worldSize.x)+coord.x;
}

ivec2 GetWorldIndexToiVec2(int id)
{
    return ivec2(id%int(worldSize.x), id/int(worldSize.x));
}

void main() {

  //ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
  int coordIndex = int(gl_GlobalInvocationID.x);
  ivec2 coord=GetWorldIndexToiVec2(coordIndex);
  //int coordIndex=GetTextureIndex(int(coord.x),int(coord.y));
  int selfIdx=coordIndex;

  imageStore(distinationTexture, coord , vec4(0.0, 0.0, 0.0, 1.0));

  distMapData[coordIndex].coord= vec2(-1.0);
  distMapData[coordIndex].dist=vec2(1e20);
  //distMapData[coordIndex].dist.x=999999999.;
  //distMapData[coordIndex].dist.y=999999999.;


}
