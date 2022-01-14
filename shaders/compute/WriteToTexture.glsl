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

layout(rgba16f) uniform image2D destination;

int GetIndex(int x, int y)
{
    return y*mapSize.x+x;
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
    return int(coord.y*worldSize.x+coord.x);
}

int GetIndexMap(int i, int x, int y)
{
    int sindex=y*mapSize.x+x;
    return sindex*maxCellSize+i;
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


void main() {

  int indx = int(gl_GlobalInvocationID.x);
  if(indx>=numPoints) return;
  /*
  ivec2 index = ivec2(pointData[indx].coord);
	imageStore(destination, index, vec4(1.0f, 1.0f, 1.0f, 1.0f));
  */
  ivec2 index = i2xy(indx);

  int texIndex=GetTextureIndex(int(pointData[indx].coord.x),int(pointData[indx].coord.y));
  distMapData[texIndex].coord=vec2(pointData[indx].coord);
  distMapData[texIndex].dist=vec2(0.0);
  distMapData[texIndex].index.x=indx;//pointData[indx].force.x;
  distMapData[texIndex].index.y=pointData[indx].force.x;
  distMapData[texIndex].nb.x=indx;
  distMapData[texIndex].nb.y=indx;
  distMapData[texIndex].nb.z=indx;
  distMapData[texIndex].nb.w=indx;

  //imageStore(destination, tex, vec4(pointData[indx].coord.x/worldSize.x, pointData[indx].coord.y/worldSize.y, 1.0f, 1.0f));

  imageStore(destination, ivec2(int(pointData[indx].coord.x),int(pointData[indx].coord.y)),
                            vec4(1.0, 1.0, 1.0f, 1.0f));

}
